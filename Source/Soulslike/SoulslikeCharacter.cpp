// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoulslikeCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "SoulslikePlayerState.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Abilities/SLSkillTypes.h"
#include "Combat/SLLockOnComponent.h"
#include "Weapons/SLWeaponTypes.h"
#include "Weapons/SLWeaponBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffect.h"
#include "SoulslikePlayerController.h"
#include "Soulslike.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

ASoulslikeCharacter::ASoulslikeCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Lock-on logic component — drives controller rotation while a target is held.
	LockOnComponent = CreateDefaultSubobject<USLLockOnComponent>(TEXT("LockOnComponent"));

	CurrentWeapon = nullptr;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)


	ComboManager_Katana_Base = CreateDefaultSubobject<USL_ComboManager>(TEXT("ComboManager_Katana_Base"));
	ComboManager_Katana_Special = CreateDefaultSubobject<USL_OneShotManager>(TEXT("ComboManager_Katana_Special"));

	ComboManager_SAS_Base = CreateDefaultSubobject<USL_ComboManager>(TEXT("ComboManager_SAS_Base"));
	ComboManager_SAS_Special = CreateDefaultSubobject<USL_OneShotManager>(TEXT("ComboManager_SAS_Special"));

	ComboManager_HS_Base = CreateDefaultSubobject<USL_ComboManager>(TEXT("ComboManager_HS_Base"));
	ComboManager_HS_Special = CreateDefaultSubobject<USL_OneShotManager>(TEXT("ComboManager_HS_Special"));
}

bool ASoulslikeCharacter::IsFalling()
{
	return GetCharacterMovement()->IsFalling();
}

void ASoulslikeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASoulslikeCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ASoulslikeCharacter::OnMoveStopped);

		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ASoulslikeCharacter::Look);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASoulslikeCharacter::Look);

		// bind for all Melee set
		TObjectPtr<UEnhancedInputComponent> EIC = Cast<UEnhancedInputComponent>(InputComponent);
		if (!EIC) {
			UE_LOG(LogTemp, Display, TEXT("[SL debug] !!! SetupPlayerInputComponent() : EnhancedInputComponent not found"));
			return;
		}

		if (!SLDA_MeleeCombat) {
			UE_LOG(LogTemp, Display, TEXT("[SL debug] !!! SetupPlayerInputComponent() : SLDA_MeleeCombat is null"));
			return;
		}

		for (const TObjectPtr<USLDA_WeaponStyle> eachWeaponStyle : SLDA_MeleeCombat->weaponStyle_list) {
			if (!eachWeaponStyle) continue;

			UE_LOG(LogTemp, Display,
				TEXT("[SL debug] SetupPlayerInputComponent() : weapon style = %s IA binding start"),
				*eachWeaponStyle->tag_weapon.ToString());

			for (const TObjectPtr<USLDA_WeaponCombo> eachCombo : eachWeaponStyle->combo_list) {
				if (!eachCombo || !eachCombo->IA_combo) continue;

				// katana input binding
				if (eachCombo == ComboManager_Katana_Base->combo) {
					EIC->BindAction(eachCombo->IA_combo, ETriggerEvent::Started,
						ComboManager_Katana_Base.Get(), &USL_ComboManager::OnCharacterInput);
				}
				if (eachCombo == ComboManager_Katana_Special->combo) {
					EIC->BindAction(eachCombo->IA_combo, ETriggerEvent::Started,
						ComboManager_Katana_Special.Get(), &USL_OneShotManager::OnCharacterInput);
				}

				// SAS input binding
				if (eachCombo == ComboManager_SAS_Base->combo) {
					EIC->BindAction(eachCombo->IA_combo, ETriggerEvent::Started,
						ComboManager_SAS_Base.Get(), &USL_ComboManager::OnCharacterInput);
				}
				if (eachCombo == ComboManager_SAS_Special->combo) {
					EIC->BindAction(eachCombo->IA_combo, ETriggerEvent::Started,
						ComboManager_SAS_Special.Get(), &USL_OneShotManager::OnCharacterInput);
				}

				// HS input binding
				if (eachCombo == ComboManager_HS_Base->combo) {
					EIC->BindAction(eachCombo->IA_combo, ETriggerEvent::Started,
						ComboManager_HS_Base.Get(), &USL_ComboManager::OnCharacterInput);
				}
				if (eachCombo == ComboManager_HS_Special->combo) {
					EIC->BindAction(eachCombo->IA_combo, ETriggerEvent::Started,
						ComboManager_HS_Special.Get(), &USL_OneShotManager::OnCharacterInput);
				}

				UE_LOG(LogTemp, Display,
					TEXT("[SL debug] SetupPlayerInputComponent() : weapon combo IA binding for tag = %s completed"),
					*eachCombo->tag_combo.ToString());
			}
		}
		if (JumpAction) {
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ASoulslikeCharacter::Jump);
		}

		if (SkillOneAction)
		{
			EnhancedInputComponent->BindAction(SkillOneAction, ETriggerEvent::Started, this, &ASoulslikeCharacter::SkillOne);
		}
		if (SkillTwoAction)
		{
			EnhancedInputComponent->BindAction(SkillTwoAction, ETriggerEvent::Started, this, &ASoulslikeCharacter::SkillTwo);
		}

		if (DodgeAction)
		{
			EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &ASoulslikeCharacter::Dodge);
		}

		if (LockOnAction)
		{
			EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Started, this, &ASoulslikeCharacter::LockOnToggle);
		}
	}
}

void ASoulslikeCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ASoulslikePlayerState* ps = GetPlayerState<ASoulslikePlayerState>();
	ASC = ps->GetAbilitySystemComponent();
	ensureOrQuit(ASC);

	ASC->InitAbilityActorInfo(ps, this);
	ps->AddDefaultAbilities();

	if (HasAuthority())
	{
		for (auto Ability : StartingAbilities)
		{
			if (Ability)
			{
				ASC->GiveAbility(FGameplayAbilitySpec(Ability, 1));
			}
		}

		for (auto Effect : StartingEffectClasses)
		{
			FGameplayEffectContextHandle Ctx = ASC->MakeEffectContext();
			Ctx.AddSourceObject(this);
			FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(Effect, 1.f, Ctx);
			if (Spec.IsValid())
			{
				ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
			}
		}

		if (StartingWeapon)
		{
			EquipWeapon(StartingWeapon);
		}
	}
}

void ASoulslikeCharacter::EquipWeapon(TSubclassOf<ASLWeaponBase> WeaponClass)
{
	if (!WeaponClass) return;

	if (CurrentWeapon)
	{
		CurrentWeapon->Unequip();
		CurrentWeapon->Destroy();
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	CurrentWeapon = GetWorld()->SpawnActor<ASLWeaponBase>(WeaponClass, SpawnParams);
	if (CurrentWeapon)
	{
		CurrentWeapon->Equip(this);
	}
}

void ASoulslikeCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	USLLockOnComponent* LockOn = GetLockOnComponent();
	if (LockOn && LockOn->GetLockedTarget())
	{
		AActor* target = LockOn->GetLockedTarget();
		FVector StartLocation = GetActorLocation();
		FVector TargetLocation = target->GetActorLocation();
		
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation);
		
		FRotator CurrentRotation = GetActorRotation();
		FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, LookAtRotation, DeltaSeconds, 5.0f);
		SetActorRotation(FRotator(0.f, SmoothedRotation.Yaw, 0.f));
	}
}

void ASoulslikeCharacter::Move(const FInputActionValue& Value)
{
	if (IsDead())
	{
		return; // dead characters do not accept movement input
	}

	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (!MovementVector.IsNearlyZero()) {
		if (!ASC->HasMatchingGameplayTag(tag_tryingToMove)) {
			ASC->AddLooseGameplayTag(tag_tryingToMove); // tag for informing whether player is trying to move
		}
	}

	if (ASC->HasMatchingGameplayTag(tag_RootMotion)) {
		return; // don't move while attacking 
	}

	if (!ASC->HasMatchingGameplayTag(tag_isMoving)) {
		ASC->AddLooseGameplayTag(tag_isMoving);
		delegate_CharacterMove.Broadcast();
	}

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void ASoulslikeCharacter::OnMoveStopped(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("[SL debug] OnMoveStopped() : player input for movement is stopped"));

	if (ASC->HasMatchingGameplayTag(tag_tryingToMove)) {
		ASC->RemoveLooseGameplayTag(tag_tryingToMove);
	}

	if (ASC->HasMatchingGameplayTag(tag_isMoving)) {
		ASC->RemoveLooseGameplayTag(tag_isMoving);
	}
}

void ASoulslikeCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

UAbilitySystemComponent* ASoulslikeCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}

void ASoulslikeCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void ASoulslikeCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ASoulslikeCharacter::SkillOne()
{
	DoActivateSkill(ESLSkillSlot::SkillOne);
}

void ASoulslikeCharacter::SkillTwo()
{
	DoActivateSkill(ESLSkillSlot::SkillTwo);
}

void ASoulslikeCharacter::Dodge()
{
	DoDodge();
}

void ASoulslikeCharacter::LockOnToggle()
{
	if (LockOnComponent)
	{
		LockOnComponent->ToggleLockOn();
		GetCharacterMovement()->bOrientRotationToMovement = !GetCharacterMovement()->bOrientRotationToMovement;
	}
}

void ASoulslikeCharacter::DoDodge()
{
	const FGameplayTag DodgeTag = FGameplayTag::RequestGameplayTag(SLCombatTags::Activate_Dodge, /*ErrorIfNotFound*/ false);
	if (DodgeTag.IsValid())
	{
		ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(DodgeTag));
	}
}

bool ASoulslikeCharacter::IsDead() const
{
	const FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag(SLCombatTags::State_Dead, /*ErrorIfNotFound*/ false);
	return DeadTag.IsValid() && ASC->HasMatchingGameplayTag(DeadTag);
}

void ASoulslikeCharacter::DoActivateSkill(ESLSkillSlot Slot)
{
	FName TagName;
	switch (Slot)
	{
	case ESLSkillSlot::SkillOne:	TagName = SLSkillTags::Activate_SkillOne; break;
	case ESLSkillSlot::SkillTwo:	TagName = SLSkillTags::Activate_SkillTwo; break;
	default: return;
	}

	FGameplayTag SkillTag = FGameplayTag::RequestGameplayTag(TagName, /*ErrorIfNotFound*/ false);
	if (!SkillTag.IsValid()) { return; }

	ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(SkillTag));
}
