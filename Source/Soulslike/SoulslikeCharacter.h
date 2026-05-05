// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Abilities/SLSkillTypes.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbility.h"
#include "SoulslikePlayerController.h"

#include "SoulslikeCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
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
	
protected:

	UPROPERTY(EditAnywhere, Category = "SL Input")
	FGameplayTag tag_tryingToMove;

	UPROPERTY(EditAnywhere, Category = "SL Input")
	FGameplayTag tag_isAttacking;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="SL Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="SL Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="SL Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="SL Input")
	UInputAction* MouseLookAction;

	UPROPERTY(EditAnywhere, Category = "SL Input")
	USLDA_MeleeControlStyles* SLDA_MeleeControlStyles;

	/** Skill slot 1 input action. */
	UPROPERTY(EditAnywhere, Category = "SL Input")
	UInputAction* SkillOneAction;

	/** Skill slot 2 input action. */
	UPROPERTY(EditAnywhere, Category = "SL Input")
	UInputAction* SkillTwoAction;



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
	void OnMoveStopped(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void MeleeAction(const FGameplayTag ability_tag);

	/** Input handlers — route to DoActivateSkill with the corresponding slot. */
	void SkillOne();
	void SkillTwo();

public:

	UFUNCTION(BlueprintCallable, Category = "GAS")
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	/** Activate the skill bound to the given slot. Safe to call from BP / UI. */
	UFUNCTION(BlueprintCallable, Category = "Input|Skill")
	virtual void DoActivateSkill(ESLSkillSlot Slot);

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

