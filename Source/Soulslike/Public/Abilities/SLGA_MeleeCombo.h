

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

UCLASS()
class SOULSLIKE_API USLAT_MeeleComboChecker : public UAbilityTask
{
    GENERATED_BODY()
public:
    // This allows the GA to create the task easily
    static USLAT_MeeleComboChecker* Create(UGameplayAbility* OwningAbility, 
        const FGameplayTag tag_comboAvailable, const FGameplayTag tag_comboGrant, const FGameplayTag tag_comboPerform,
        const FGameplayTag tag_isMoving, const FGameplayTag tag_interrupt, FGameplayTagContainer comboActionTagContainer);

    virtual void TickTask(float DeltaTime) override;

    void CheckPlayerMoveInterrupt(TObjectPtr<UAbilitySystemComponent> asc);

protected:
    FGameplayTag tag_comboAvailable;
    FGameplayTag tag_comboGrant;
    FGameplayTag tag_comboPerform;

    FGameplayTag tag_isMoving;
    FGameplayTag tag_interrupt;

    FGameplayTagContainer comboActionTagContainer;

public:
    FComboInterruptDelegate OnInterruptDetected;
};

UENUM()
enum class ESL_MeleeComboState : uint8 {

};

UCLASS(abstract)
class SOULSLIKE_API USLGA_MeleeCombo : public UGameplayAbility
{
	GENERATED_BODY()

protected:

    UPROPERTY(EditDefaultsOnly, Category = "Combo|Event&State")
    FGameplayTag tag_inputAsComboStart;
    UPROPERTY(EditDefaultsOnly, Category = "Combo|Event&State")
    FGameplayTag tag_inputAsComboEnd;
    UPROPERTY(EditDefaultsOnly, Category = "Combo|Event&State")
    FGameplayTag tag_comboPerform;
    UPROPERTY(EditDefaultsOnly, Category = "Combo|Event")
    FGameplayTag tag_interrupt;
    UPROPERTY(EditDefaultsOnly, Category = "Combo|State")
    FGameplayTag tag_comboAvailable;
    UPROPERTY(EditDefaultsOnly, Category = "Combo|State")
    FGameplayTag tag_comboGrant;
    UPROPERTY(EditDefaultsOnly, Category = "Combo|State")
    FGameplayTag tag_comboLastAction;

    UPROPERTY(EditDefaultsOnly, Category = "Combo|State")
    FGameplayTag tag_isMoving;

    UPROPERTY(EditDefaultsOnly, Category = "Combo|DataAsset")
    TObjectPtr<USLDA_WeaponCombo> SLDA_WeaponCombo;
    int currentActionIdx;
    int lastActionIdx;
    
    USLAT_MeeleComboChecker* comboChecker = nullptr;
    FGameplayTagContainer comboActionTagContainer;
    

private:
    void addTag(TObjectPtr<UAbilitySystemComponent> asc, FGameplayTag tag);
    void removeTag(TObjectPtr<UAbilitySystemComponent> asc, FGameplayTag tag);

    void StartComboChcker();
    void EndComboChecker();

protected:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

    void SimpleEndAbility();

    void StartAction();

    UFUNCTION()
    void OnInputAsComboStart(FGameplayEventData Payload);
    UFUNCTION()
    void OnInputAsComboEnd(FGameplayEventData Payload);
    UFUNCTION()
    void OnComboAvailable(FGameplayEventData Payload);
    UFUNCTION()
    void OnComboPerform(FGameplayEventData Payload);
    UFUNCTION()
    void OnInterrupt(FGameplayEventData Payload);

};
