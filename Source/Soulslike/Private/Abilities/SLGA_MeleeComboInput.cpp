


#include "Abilities/SLGA_MeleeComboInput.h"

void USLGA_MeleeComboInput::addTag(TObjectPtr<UAbilitySystemComponent> asc, FGameplayTag tag)
{
	if (!asc->HasMatchingGameplayTag(tag)) {
		asc->AddLooseGameplayTag(tag);
	}
}

void USLGA_MeleeComboInput::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	TObjectPtr<UAbilitySystemComponent> asc = GetAbilitySystemComponentFromActorInfo();
	if (!asc) {
		UE_LOG(LogTemp, Display, TEXT("[SL debug] !!! %s ActivateAbility() : no ASC"), *this->GetName());
		return;
	}
	if (!SLDA_WeaponCombo) {
		UE_LOG(LogTemp, Display, TEXT("[SL debug] !!! %s ActivateAbility() : no SLDA_WeaponCombo"),
			*this->GetName());
		return;
	}

	tag_combo = SLDA_WeaponCombo->tag_combo;
	tag_inputAsComboStart = SLDA_WeaponCombo->tag_inputAsComboStart;
	tag_inputAsComboEnd = SLDA_WeaponCombo->tag_inputAsComboEnd;
	tag_comboGrant = SLDA_WeaponCombo->tag_comboGrant;
	tag_comboLastAction = SLDA_WeaponCombo->tag_comboLastAction;

	if (!asc->HasMatchingGameplayTag(tag_combo)) {
		asc->TryActivateAbilityByClass(SLDA_WeaponCombo->GA_Input);
		return;
	}

	if (asc->HasMatchingGameplayTag(tag_inputAsComboStart)) {
		addTag(asc, tag_comboGrant);
		return;
	}

	if (asc->HasMatchingGameplayTag(tag_inputAsComboEnd)) {
		asc->TryActivateAbilityByClass(SLDA_WeaponCombo->GA_Input);
		return;
	}
}

void USLGA_MeleeComboInput::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
