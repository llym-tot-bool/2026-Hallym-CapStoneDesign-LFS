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
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffect.h"
#include "SoulslikePlayerController.h"

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
	GetCharacterMovement()->JumpZVelocity = 500.f;
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

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ASoulslikeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASoulslikeCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ASoulslikeCharacter::OnMoveStopped);

		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ASoulslikeCharacter::Look);

		// Looking
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
			UE_LOG(LogTemp, Display,
				TEXT("[SL debug] SetupPlayerInputComponent() : weapon style = %s IA binding start"),
				*eachWeaponStyle->tag_weapon.ToString());

			for (const TObjectPtr<USLDA_WeaponCombo> eachCombo : eachWeaponStyle->combo_list) {
				EIC->BindAction(eachCombo->IA_combo, ETriggerEvent::Started,
					this, &ASoulslikeCharacter::MeleeAction,
					eachCombo);

				UE_LOG(LogTemp, Display,
					TEXT("[SL debug] SetupPlayerInputComponent() : weapon combo IA binding for tag = %s completed"),
					*eachCombo->tag_combo.ToString());
			}
		}

		// skills
		if (SkillOneAction)
		{
			EnhancedInputComponent->BindAction(SkillOneAction, ETriggerEvent::Started, this, &ASoulslikeCharacter::SkillOne);
		}
		if (SkillTwoAction)
		{
			EnhancedInputComponent->BindAction(SkillTwoAction, ETriggerEvent::Started, this, &ASoulslikeCharacter::SkillTwo);
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
		ps->GetAbilitySystemComponent()->InitAbilityActorInfo(ps, this);

		ps->AddDefaultAbilities();
	}
}

void ASoulslikeCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	TObjectPtr<UAbilitySystemComponent> asc = GetAbilitySystemComponent();
	if (!asc) {
		UE_LOG(LogTemp, Display, TEXT("[SL debug] Move() : ASC is null"));
		return;
	}


	if (!MovementVector.IsNearlyZero()) {
		if (!asc->HasMatchingGameplayTag(tag_tryingToMove)) {
			asc->AddLooseGameplayTag(tag_tryingToMove); // tag for informing whether player is trying to move
		}
	}

	if (asc->HasMatchingGameplayTag(tag_isAttacking)) {
		return; // don't move while attacking 
	}

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void ASoulslikeCharacter::OnMoveStopped(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("[SL debug] OnMoveStopped() : player input for movement is stopped"));
	TObjectPtr<UAbilitySystemComponent> asc = GetAbilitySystemComponent();
	if (!asc) {
		UE_LOG(LogTemp, Display, TEXT("[SL debug] Move() : ASC is null"));
		return;
	}

	if (asc->HasMatchingGameplayTag(tag_tryingToMove)) {
		asc->RemoveLooseGameplayTag(tag_tryingToMove);
	}
}

void ASoulslikeCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ASoulslikeCharacter::MeleeAction(const TObjectPtr<USLDA_WeaponCombo> combo)
{
	UE_LOG(LogTemp, Display, 
		TEXT("[SL debug] MeleeAction() : ability tag = %s"),
		*combo->tag_combo.ToString());

	TObjectPtr<UAbilitySystemComponent> asc = GetAbilitySystemComponent();

	if (!asc) { UE_LOG(LogTemp, Display, TEXT("[SL debug] !!! MeleeAction() : no ASC")); return; }

	if (asc->HasMatchingGameplayTag(combo->tag_combo)) {
		if (asc->HasMatchingGameplayTag(combo->tag_inputAsComboStart) &&
			!asc->HasMatchingGameplayTag(combo->tag_comboGrant)) {
			UE_LOG(LogTemp, Display, TEXT("[SL debug] MeleeAction() : added combo grant = %s"),
				*combo->tag_comboGrant.ToString());
			asc->AddLooseGameplayTag(combo->tag_comboGrant);
		}
	}
	else {
		asc->TryActivateAbilitiesByTag(FGameplayTagContainer(combo->tag_combo));
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

void ASoulslikeCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void ASoulslikeCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void ASoulslikeCharacter::SkillOne()
{
	DoActivateSkill(ESLSkillSlot::SkillOne);
}

void ASoulslikeCharacter::SkillTwo()
{
	DoActivateSkill(ESLSkillSlot::SkillTwo);
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
