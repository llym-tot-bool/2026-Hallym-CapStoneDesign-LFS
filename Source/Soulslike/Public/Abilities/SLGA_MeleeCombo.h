

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/SLGA_MeleeSweep.h"

#include "SLGA_MeleeCombo.generated.h"

/**
 * 
 */

UCLASS()
class SOULSLIKE_API USLAT_MeeleComboChecker : public UAbilityTask
{
    GENERATED_BODY()
public:
    // This allows the GA to create the task easily
    static USLAT_MeeleComboChecker* Create(UGameplayAbility* OwningAbility, 
        const FGameplayTag tag_comboAvailable, const FGameplayTag tag_comboGrant, const FGameplayTag tag_comboPerform);

    virtual void TickTask(float DeltaTime) override;

protected:
    FGameplayTag tag_comboAvailable;
    FGameplayTag tag_comboGrant;
    FGameplayTag tag_comboPerform;
};

UCLASS(abstract)
class SOULSLIKE_API USLGA_MeleeCombo : public UGameplayAbility
{
	GENERATED_BODY()

protected:

    UPROPERTY(EditDefaultsOnly, Category = "Combo|Event")
    FGameplayTag tag_inputAsComboStart;
    UPROPERTY(EditDefaultsOnly, Category = "Combo|Event")
    FGameplayTag tag_inputAsComboEnd;
    UPROPERTY(EditDefaultsOnly, Category = "Combo|Event")
    FGameplayTag tag_comboPerform;
    UPROPERTY(EditDefaultsOnly, Category = "Combo|State")
    FGameplayTag tag_comboAvailable;
    UPROPERTY(EditDefaultsOnly, Category = "Combo|State")
    FGameplayTag tag_comboGrant;

    USLAT_MeeleComboChecker* comboChecker = nullptr;

    TArray<TObjectPtr<USLGA_MeleeSweep>> comboActoin_list;
    int currentActionIdx;



protected:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

    void InputAsComboStart(FGameplayEventData Payload);
    void InputAsComboEnd(FGameplayEventData Payload);

    void ComboPerform(FGameplayEventData Payload);

};
