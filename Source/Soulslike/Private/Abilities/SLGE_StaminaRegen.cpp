#include "Abilities/SLGE_StaminaRegen.h"
#include "SLCharacterAttributeSet.h"
#include "Weapons/SLWeaponTypes.h"

USLGE_StaminaRegen::USLGE_StaminaRegen()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	
	// Fast regen: tick 10 times per second
	Period = 0.1f;
	PeriodicInhibitionPolicy = EGameplayEffectPeriodInhibitionPolicy::NeverReset;

	// Modifier: Stamina += Source.StaminaRegen
	FGameplayModifierInfo StaminaMod;
	StaminaMod.Attribute = USLCharacterAttributeSet::GetStaminaAttribute();
	StaminaMod.ModifierOp = EGameplayModOp::Additive;
	
	FAttributeBasedFloat StaminaRegenMagnitude;
	StaminaRegenMagnitude.AttributeToCapture = USLCharacterAttributeSet::GetStaminaRegenAttribute();
	StaminaRegenMagnitude.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	StaminaMod.ModifierMagnitude = FGameplayEffectModifierMagnitude(StaminaRegenMagnitude);

	Modifiers.Add(StaminaMod);

	// This effect is ignored while the target has the State.Stamina.Spending tag.
	// The attribute set applies this tag whenever a stamina cost is paid.
	OngoingTagRequirements.IgnoreTags.AddTag(FGameplayTag::RequestGameplayTag(SLCombatTags::State_StaminaSpending));
}
