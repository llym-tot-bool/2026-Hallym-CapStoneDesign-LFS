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
#include "Soulslike.h"
#include "SoulslikePlayerState.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Abilities/SLSkillTypes.h"
#include "Abilities/SLGE_StaminaCost.h"
#include "Abilities/SLGE_StaminaRegen.h"
#include "SLCharacterAttributeSet.h"
#include "Combat/SLLockOnComponent.h"
#include "Weapons/SLWeaponTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffect.h"

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

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ASoulslikeCharacter::PerformWeaponTrace()
{
	FVector start = GetMesh()->GetSocketLocation("Socket_weapon_base");
	FVector end = GetMesh()->GetSocketLocation("Socket_weapon_tip");

	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add(this);
	FHitResult hitresult;

	bool bHit = UKismetSystemLibrary::SphereTraceSingle(
		this, start, end, 15.0f,
		UEngineTypes::ConvertToTraceType(ECC_Pawn),
		false, actorsToIgnore, EDrawDebugTrace::ForDuration,
		hitresult, true
	);

	if (bHit && hitresult.GetActor()) {
		AActor* hitActor = hitresult.GetActor();

		if (!AlreadyHitActors.Contains(hitActor)) {
			AlreadyHitActors.Add(hitActor);

			IAbilitySystemInterface* hitInterface = Cast<IAbilitySystemInterface>(hitActor);
			if (hitInterface) {
				UAbilitySystemComponent* targetASC = hitInterface->GetAbilitySystemComponent();
				UAbilitySystemComponent* myASC = GetAbilitySystemComponent();

				if (targetASC && myASC) {
					FGameplayEffectContextHandle effectHandle = myASC->MakeEffectContext();
					effectHandle.AddHitResult(hitresult);

					FGameplayEventData payload;
					payload.Instigator = this;
					payload.Target = hitActor;
					payload.ContextHandle = effectHandle;

					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, FGameplayTag::RequestGameplayTag(FName("Event.Combat.HitLanded")), payload);
					GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Hit Landed!"));
				}
			}


		}
	}
}

void ASoulslikeCharacter::ClearHitList()
{
	AlreadyHitActors.Empty();
}

void ASoulslikeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASoulslikeCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ASoulslikeCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASoulslikeCharacter::Look);

		// light attack
		EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Started, this, &ASoulslikeCharacter::LightAttack);

		// skills
		if (SkillOneAction)
		{
			EnhancedInputComponent->BindAction(SkillOneAction, ETriggerEvent::Started, this, &ASoulslikeCharacter::SkillOne);
		}
		if (SkillTwoAction)
		{
			EnhancedInputComponent->BindAction(SkillTwoAction, ETriggerEvent::Started, this, &ASoulslikeCharacter::SkillTwo);
		}

		// dodge
		if (DodgeAction)
		{
			EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &ASoulslikeCharacter::Dodge);
		}

		// lock-on
		if (LockOnAction)
		{
			EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Started, this, &ASoulslikeCharacter::LockOnToggle);
		}
	}
	else
	{
		UE_LOG(LogSoulslike, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ASoulslikeCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ASoulslikePlayerState* ps = GetPlayerState<ASoulslikePlayerState>();
	if (ps) {
		UAbilitySystemComponent* ASC = ps->GetAbilitySystemComponent();
		ASC->InitAbilityActorInfo(ps, this);

		ps->AddDefaultAbilities();

		// Apply the always-on stamina regen GE. It pauses itself via
		// OngoingTagRequirements while State.Stamina.Spending is on the ASC.
		if (ASC && HasAuthority())
		{
			FGameplayEffectContextHandle Ctx = ASC->MakeEffectContext();
			Ctx.AddSourceObject(this);
			FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(USLGE_StaminaRegen::StaticClass(), 1.f, Ctx);
			if (Spec.IsValid())
			{
				ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
			}
		}
	}
}

void ASoulslikeCharacter::Move(const FInputActionValue& Value)
{
	if (IsDead())
	{
		return; // dead characters do not accept movement input
	}

	if (ASoulslikePlayerState* ps = GetPlayerState<ASoulslikePlayerState>()) {
		if (ps->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Attacking")))) {
			return; // don't move while attacking
		}
	}

	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void ASoulslikeCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ASoulslikeCharacter::LightAttack()
{
	if (ASoulslikePlayerState* ps = GetPlayerState<ASoulslikePlayerState>()) {
		UAbilitySystemComponent* ASC = ps->GetAbilitySystemComponent();

		if (ASC) {
			FGameplayTag attackTag = FGameplayTag::RequestGameplayTag(FName("PlayerAbility.Attack.Light"));
			ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(attackTag));
		}
	}
}

UAbilitySystemComponent* ASoulslikeCharacter::GetAbilitySystemComponent() const
{
	ASoulslikePlayerState* ps = GetPlayerState<ASoulslikePlayerState>();
	return ps ? ps->GetAbilitySystemComponent() : nullptr;
}

void ASoulslikeCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void ASoulslikeCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ASoulslikeCharacter::DoLightAttack()
{
	LightAttack();
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
	}
}

void ASoulslikeCharacter::DoDodge()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		const FGameplayTag DodgeTag = FGameplayTag::RequestGameplayTag(SLCombatTags::Activate_Dodge, /*ErrorIfNotFound*/ false);
		if (DodgeTag.IsValid())
		{
			ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(DodgeTag));
		}
	}
}

bool ASoulslikeCharacter::ApplyStaminaCost(float Cost)
{
	if (Cost <= 0.f) { return false; }

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC) { return false; }

	FGameplayEffectContextHandle Ctx = ASC->MakeEffectContext();
	Ctx.AddSourceObject(this);
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(USLGE_StaminaCost::StaticClass(), 1.f, Ctx);
	if (!SpecHandle.IsValid()) { return false; }

	const FGameplayTag CostTag = FGameplayTag::RequestGameplayTag(SLCombatTags::SetByCaller_StaminaCost, /*ErrorIfNotFound*/ false);
	if (CostTag.IsValid())
	{
		// Pass negative so the additive modifier subtracts from Stamina.
		SpecHandle.Data->SetSetByCallerMagnitude(CostTag, -Cost);
	}
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	return true;
}

bool ASoulslikeCharacter::HasEnoughStamina(float RequiredAmount) const
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		return ASC->GetNumericAttribute(USLCharacterAttributeSet::GetStaminaAttribute()) >= RequiredAmount;
	}
	return false;
}

bool ASoulslikeCharacter::IsDead() const
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		const FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag(SLCombatTags::State_Dead, /*ErrorIfNotFound*/ false);
		return DeadTag.IsValid() && ASC->HasMatchingGameplayTag(DeadTag);
	}
	return false;
}

void ASoulslikeCharacter::DoActivateSkill(ESLSkillSlot Slot)
{
	ASoulslikePlayerState* ps = GetPlayerState<ASoulslikePlayerState>();
	if (!ps) { return; }

	UAbilitySystemComponent* ASC = ps->GetAbilitySystemComponent();
	if (!ASC) { return; }

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
