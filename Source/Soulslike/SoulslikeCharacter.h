// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Abilities/SLSkillTypes.h"
#include "AbilitySystemInterface.h"

#include "SoulslikeCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class USLLockOnComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class ASoulslikeCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** Lock-on logic component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USLLockOnComponent* LockOnComponent;

protected: 
	
	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

	// light attack action IA_light_attack
	UPROPERTY(EditAnywhere, Category = "Input");
	UInputAction* LightAttackAction;

	/** Skill slot 1 input action. */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* SkillOneAction;

	/** Skill slot 2 input action. */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* SkillTwoAction;

	/** Dodge / roll input action — drives PlayerAbility.Dodge. */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* DodgeAction;

	/** Toggle lock-on input action (single press toggles). */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LockOnAction;

	// The list of enemies hit during the current swing
	UPROPERTY()
	TArray<AActor*> AlreadyHitActors;

public:

	/** Constructor */
	ASoulslikeCharacter();	


protected:

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// link playerstate when possessed
	virtual void PossessedBy(AController* NewController) override;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	// ligth attack
	void LightAttack();

	/** Input handlers — route to DoActivateSkill with the corresponding slot. */
	void SkillOne();
	void SkillTwo();

	/** Dodge input handler — activates the dodge ability. */
	void Dodge();

	/** Lock-on toggle input handler. */
	void LockOnToggle();

public:

	UFUNCTION(BlueprintCallable, Category = "GAS")
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	// handle light attack
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoLightAttack();

	/** Activate the skill bound to the given slot. Safe to call from BP / UI. */
	UFUNCTION(BlueprintCallable, Category = "Input|Skill")
	virtual void DoActivateSkill(ESLSkillSlot Slot);

	/** Activate the dodge ability via its activation tag. Safe to call from BP / UI. */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoDodge();

	/**
	 * Apply a stamina cost to this character via USLGE_StaminaCost. Pass a POSITIVE
	 * cost — the helper negates internally before packing the SetByCaller magnitude.
	 * Returns true if the cost was applied (i.e. the character had an ASC).
	 */
	UFUNCTION(BlueprintCallable, Category = "Combat|Stamina")
	virtual bool ApplyStaminaCost(float Cost);

	/**
	 * Returns true if Stamina >= RequiredAmount. Lightweight check for combat
	 * abilities to call before committing.
	 */
	UFUNCTION(BlueprintPure, Category = "Combat|Stamina")
	bool HasEnoughStamina(float RequiredAmount) const;

	/** True once the lethal hit has set State.Dead on this actor's ASC. */
	UFUNCTION(BlueprintPure, Category = "Combat|Status")
	bool IsDead() const;

	// visual trace
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void PerformWeaponTrace();

	// Clears the hit list (Call this when the hitbox starts)
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ClearHitList();

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Returns LockOn component **/
	FORCEINLINE USLLockOnComponent* GetLockOnComponent() const { return LockOnComponent; }
};

