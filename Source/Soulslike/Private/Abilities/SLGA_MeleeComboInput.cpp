


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
		UE_LOG(LogTemp, Display, TEXT("[SL debug] !!! melee combo GA_Input ActivateAbility() : no ASC"));
		return;
	}
	if (!SLDA_WeaponCombo) {
		UE_LOG(LogTemp, Display, TEXT("[SL debug] !!! melee combo GA_Input ActivateAbility() : no SLDA_WeaponCombo"));
		return;
	}

	tag_combo = SLDA_WeaponCombo->tag_combo;
	tag_inputAsComboStart = SLDA_WeaponCombo->tag_inputAsComboStart;
	tag_inputAsComboEnd = SLDA_WeaponCombo->tag_inputAsComboEnd;
	tag_comboGrant = SLDA_WeaponCombo->tag_comboGrant;
	tag_comboLastAction = SLDA_WeaponCombo->tag_comboLastAction;

	if (asc->HasMatchingGameplayTag(tag_inputAsComboStart)) {
		UE_LOG(LogTemp, Display,
			TEXT("[SL debug] melee combo GA_Input ActivateAbility() : set tag_comboGrant"));
		addTag(asc, tag_comboGrant);
		return;
	}

	if (!asc->HasMatchingGameplayTag(tag_combo)) {
		if (SLDA_WeaponCombo->GA_list.Num() > 0) {
			UE_LOG(LogTemp, Display,
				TEXT("[SL debug] melee combo GA_Input ActivateAbility() : activate GA_Combo"));
			asc->TryActivateAbilityByClass(SLDA_WeaponCombo->GA_Combo);
		}
		else {
			UE_LOG(LogTemp, Display,
				TEXT("[SL debug] !!! melee combo GA_Input ActivateAbility() : combo GA_list seems empty"));
		}
		return;
	}


	UE_LOG(LogTemp, Display, TEXT("[SL debug] melee combo GA_Input ActivateAbility() : input ignored"));
}

void USLGA_MeleeComboInput::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
