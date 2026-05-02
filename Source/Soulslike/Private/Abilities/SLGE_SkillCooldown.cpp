#include "Abilities/SLGE_SkillCooldown.h"
#include "Abilities/SLSkillTypes.h"
#include "GameplayEffectTypes.h"

USLGE_SkillCooldown::USLGE_SkillCooldown()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	FSetByCallerFloat SetByCaller;
	SetByCaller.DataTag = FGameplayTag::RequestGameplayTag(SLSkillTags::SetByCaller_CooldownDuration, /*ErrorIfNotFound*/ false);

	DurationMagnitude = FGameplayEffectModifierMagnitude(SetByCaller);
}
