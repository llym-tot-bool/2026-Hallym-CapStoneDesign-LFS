// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Abilities/SLSkillTypes.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbility.h"
#include "SoulslikePlayerController.h"
#include "SLDA_MeleeCombat.h"
#include "SL_ComboManager.h"
#include "SL_OneShotManager.h"
#include "SL_HitManager.h"

#include "SoulslikeCharacter.generated.h"

class ASLWeaponBase;
class UGameplayEffect;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class USLLockOnComponent;
struct FInputActionValue;
class USL_ComboManager;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(abstract, BlueprintType)
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

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "SL Tag")
	FGameplayTag tag_tryingToMove;

	UPROPERTY(EditAnywhere, Category = "SL Tag")
	FGameplayTag tag_isMoving;

	UPROPERTY(EditAnywhere, Category = "SL Tag")
	FGameplayTag tag_RootMotion;

	UPROPERTY(EditAnywhere, Category = "SL Data Asset")
	TObjectPtr<USLDA_MeleeCombat> SLDA_MeleeCombat;

public:
	ASoulslikeCharacter();

	bool IsFalling();

protected:

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;
	
	virtual void Tick(float DeltaSeconds) override;

protected:

	void Move(const FInputActionValue& Value);
	void OnMoveStopped(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	void SkillOne();
	void SkillTwo();

	void Dodge();

	void OnHit();

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

	UFUNCTION(BlueprintCallable, Category = "Combat|OnHit")
	virtual void DoOnHit();

	UFUNCTION(BlueprintPure, Category = "Combat|Status")
	bool IsDead() const;
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void EquipWeapon(TSubclassOf<ASLWeaponBase> WeaponClass);
	
	UPROPERTY(EditDefaultsOnly, Category = "Starting Setup")
	TArray<TSubclassOf<UGameplayEffect>> StartingEffectClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Starting Setup")
	TArray<TSubclassOf<UGameplayAbility>> StartingAbilities;
	
	UPROPERTY(EditDefaultsOnly, Category = "Starting Setup")
	TSubclassOf<ASLWeaponBase> StartingWeapon;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<ASLWeaponBase> CurrentWeapon;

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Returns LockOn component **/
	FORCEINLINE USLLockOnComponent* GetLockOnComponent() const { return LockOnComponent; }

public:
	UPROPERTY(BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> ASC;

	
	// on hit manager
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OnHitManager", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USL_HitManager> OnHitManager;

	// katana combo managers
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ComboManager", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USL_ComboManager> ComboManager_Katana_Base;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ComboManager", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USL_OneShotManager> ComboManager_Katana_Special;

	// SAS combo managers
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ComboManager", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USL_ComboManager> ComboManager_SAS_Base;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ComboManager", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USL_OneShotManager> ComboManager_SAS_Special;

	// HS combo managers
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ComboManager", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USL_ComboManager> ComboManager_HS_Base;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ComboManager", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USL_OneShotManager> ComboManager_HS_Special;

	FSLDLG_CharacterMove delegate_CharacterMove;

};

