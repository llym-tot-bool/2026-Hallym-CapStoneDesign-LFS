

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

    UPROPERTY(EditDefaultsOnly, Category = "Combo|Event&State")
    FGameplayTag tag_inputAsComboStart;
    UPROPERTY(EditDefaultsOnly, Category = "Combo|Event&State")
    FGameplayTag tag_inputAsComboEnd;
    UPROPERTY(EditDefaultsOnly, Category = "Combo|Event")
    FGameplayTag tag_comboPerform;
    UPROPERTY(EditDefaultsOnly, Category = "Combo|State")
    FGameplayTag tag_comboAvailable;
    UPROPERTY(EditDefaultsOnly, Category = "Combo|State")
    FGameplayTag tag_comboGrant;

    UPROPERTY(EditDefaultsOnly, Category = "Combo|DataAsset")
    TObjectPtr<USLDA_WeaponCombo> SLDA_WeaponCombo;
    int currentActionIdx;
    int lastActionIdx;
    
    USLAT_MeeleComboChecker* comboChecker = nullptr;
    

private:
    void addTag(TObjectPtr<UAbilitySystemComponent> asc, FGameplayTag tag);
    void removeTag(TObjectPtr<UAbilitySystemComponent> asc, FGameplayTag tag);

    void StartComboChcker();
    void EndChomboChecker();

protected:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

    void InputAsComboStart(FGameplayEventData Payload);
    void InputAsComboEnd(FGameplayEventData Payload);

    void ComboAvailable(FGameplayEventData Payload);

    void ComboPerform(FGameplayEventData Payload);

};
