#include "Abilities/SLGE_HealthRegen.h"
#include "SLCharacterAttributeSet.h"
#include "Weapons/SLWeaponTypes.h"

USLGE_HealthRegen::USLGE_HealthRegen()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	
	// Regen every 1 second
	Period = 1.0f;
	PeriodicInhibitionPolicy = EGameplayEffectPeriodInhibitionPolicy::NeverReset;

	// Modifier: Health += Source.HealthRegen
	FGameplayModifierInfo HealthMod;
	HealthMod.Attribute = USLCharacterAttributeSet::GetHealthAttribute();
	HealthMod.ModifierOp = EGameplayModOp::Additive;
	
	FAttributeBasedFloat HealthRegenMagnitude;
	HealthRegenMagnitude.AttributeToCapture = USLCharacterAttributeSet::GetHealthRegenAttribute();
	HealthRegenMagnitude.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	HealthMod.ModifierMagnitude = FGameplayEffectModifierMagnitude(HealthRegenMagnitude);

	Modifiers.Add(HealthMod);

	// This effect is ignored while the target has the State.RecentlyDamaged tag.
	// The attribute set applies this tag for 3 seconds whenever a damaging GE lands.
	OngoingTagRequirements.IgnoreTags.AddTag(FGameplayTag::RequestGameplayTag(SLCombatTags::State_RecentlyDamaged));
}
