

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/SLGA_MeleeSweep.h"
#include "SLDA_MeleeCombat.h"

#include "SLGA_MeleeComboInput.generated.h"
/**
 * 
 */
UCLASS()
class SOULSLIKE_API USLGA_MeleeComboInput : public UGameplayAbility
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, Category = "Combo")
	TObjectPtr<USLDA_WeaponCombo> SLDA_WeaponCombo;

	FGameplayTag tag_combo;

protected:

	void addTag(TObjectPtr<UAbilitySystemComponent> asc, FGameplayTag tag);

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

};
