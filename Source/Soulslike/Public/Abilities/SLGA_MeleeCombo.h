

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/SLGA_MeleeSweep.h"
#include "SLDA_MeleeCombat.h"

#include "SLGA_MeleeCombo.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FComboInterruptDelegate);


UCLASS(abstract)
class SOULSLIKE_API USLGA_MeleeCombo : public UGameplayAbility
{
	GENERATED_BODY()

protected:

    UPROPERTY(EditDefaultsOnly, Category = "Combo|State")
    FGameplayTag tag_isMoving;

    UPROPERTY(EditDefaultsOnly, Category = "Combo|DataAsset")
    TObjectPtr<USLDA_WeaponCombo> SLDA_WeaponCombo;
    int currentActionIdx;
    int lastActionIdx;
    
    FGameplayTag tag_combo;
    FGameplayTagContainer comboActionTagContainer;

    ESL_MeleeSweep_State state;
    TObjectPtr<USLGA_MeleeSweep> currentComboAction;

    bool bIsInputBuffered = false;

protected:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

    void SimpleEndAbility();

    void StartAction();

    void PlayNextComboAction();

    UFUNCTION()
    void OnComboInput();
    UFUNCTION()
    void OnTranslation();
    UFUNCTION()
    void OnRecovery();
    UFUNCTION()
    void OnCharacterMove();

    UFUNCTION()
    void OnPlayerInput(FGameplayTag tag);

};
