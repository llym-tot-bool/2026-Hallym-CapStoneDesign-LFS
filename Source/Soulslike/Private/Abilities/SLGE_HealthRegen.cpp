#include "Abilities/SLGE_HealthRegen.h"
#include "SLCharacterAttributeSet.h"
#include "Weapons/SLWeaponTypes.h"
#include "GameplayEffectComponents/TargetTagRequirementsGameplayEffectComponent.h"

USLGE_HealthRegen::USLGE_HealthRegen()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	
	// Regen every 1 second
	Period = 0.1f;
	PeriodicInhibitionPolicy = EGameplayEffectPeriodInhibitionRemovedPolicy::NeverReset;
	// Modifier: Health += Source.HealthRegen
	FGameplayModifierInfo HealthMod;
	HealthMod.Attribute = USLCharacterAttributeSet::GetHealthAttribute();
	HealthMod.ModifierOp = EGameplayModOp::Additive;
	
	FAttributeBasedFloat HealthRegenMagnitude;
	HealthRegenMagnitude.BackingAttribute.AttributeToCapture = USLCharacterAttributeSet::GetHealthRegenAttribute();
	HealthRegenMagnitude.BackingAttribute.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	HealthMod.ModifierMagnitude = FGameplayEffectModifierMagnitude(HealthRegenMagnitude);

	Modifiers.Add(HealthMod);

	// This effect is ignored while the target has the State.RecentlyDamaged tag.
	// The attribute set applies this tag for 3 seconds whenever a damaging GE lands.
	UTargetTagRequirementsGameplayEffectComponent* TagComponent = CreateDefaultSubobject<UTargetTagRequirementsGameplayEffectComponent>(TEXT("TagComponent"));
	if (TagComponent)
	{
		TagComponent->OngoingTagRequirements.IgnoreTags.AddTag(FGameplayTag::RequestGameplayTag(SLCombatTags::State_RecentlyDamaged));
		GEComponents.Add(TagComponent);
	}
}
