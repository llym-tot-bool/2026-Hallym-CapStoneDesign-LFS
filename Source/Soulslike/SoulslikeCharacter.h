// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Abilities/SLSkillTypes.h"
#include "AbilitySystemInterface.h"

#include "SoulslikeCharacter.generated.h"

class UGameplayEffect;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class USLLockOnComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(abstract)
class ASoulslikeCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USLLockOnComponent* LockOnComponent;

protected: 
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

	UPROPERTY(EditAnywhere, Category = "Input");
	UInputAction* LightAttackAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* SkillOneAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* SkillTwoAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* DodgeAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LockOnAction;

	UPROPERTY()
	TArray<AActor*> AlreadyHitActors;

public:

	ASoulslikeCharacter();	


protected:

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;
	
	virtual void Tick(float DeltaSeconds) override;

protected:

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	void LightAttack();

	void SkillOne();
	void SkillTwo();

	void Dodge();

	void LockOnToggle();

public:

	UFUNCTION(BlueprintCallable, Category = "GAS")
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	UFUNCTION(BlueprintCallable, Category = "Input|Skill")
	virtual void DoActivateSkill(ESLSkillSlot Slot);

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoDodge();

	UFUNCTION(BlueprintPure, Category = "Combat|Status")
	bool IsDead() const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void PerformWeaponTrace();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ClearHitList();
	
	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TArray<TSubclassOf<UGameplayEffect>> StartingEffectClasses;

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Returns LockOn component **/
	FORCEINLINE USLLockOnComponent* GetLockOnComponent() const { return LockOnComponent; }
};

