#include "Abilities/SLGE_WeaponDamage.h"

#include "SLCharacterAttributeSet.h"
#include "Weapons/SLWeaponTypes.h"

#include "GameplayEffectTypes.h"

USLGE_WeaponDamage::USLGE_WeaponDamage()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	// Modifier: Damage += SetByCaller("Data.Damage.Base")
	// PostGameplayEffectExecute on the AttributeSet reads Damage and subtracts it from Health.
	FGameplayModifierInfo DamageMod;
	DamageMod.Attribute = USLCharacterAttributeSet::GetDamageAttribute();
	DamageMod.ModifierOp = EGameplayModOp::Additive;

	FSetByCallerFloat SetByCaller;
	SetByCaller.DataTag = FGameplayTag::RequestGameplayTag(SLCombatTags::SetByCaller_DamageBase, /*ErrorIfNotFound*/ false);
	DamageMod.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCaller);

	Modifiers.Add(DamageMod);

	// Modifier: GroggyDamage += SetByCaller("Data.Damage.Poise")
	// AttributeSet consumes this meta attribute and subtracts it from Groggy gauge.
	FGameplayModifierInfo GroggyDamageMod;
	GroggyDamageMod.Attribute = USLCharacterAttributeSet::GetGroggyDamageAttribute();
	GroggyDamageMod.ModifierOp = EGameplayModOp::Additive;

	FSetByCallerFloat GroggySetByCaller;
	GroggySetByCaller.DataTag = FGameplayTag::RequestGameplayTag(SLCombatTags::SetByCaller_PoiseDamage, /*ErrorIfNotFound*/ false);
	GroggyDamageMod.ModifierMagnitude = FGameplayEffectModifierMagnitude(GroggySetByCaller);

	Modifiers.Add(GroggyDamageMod);

	// Fire the project's hit GameplayCue every time this GE applies. BP cue
	// handlers attached to GameplayCue.Combat.Hit own the visuals / audio.
	const FGameplayTag HitCueTag = FGameplayTag::RequestGameplayTag(SLCombatTags::Cue_Hit, /*ErrorIfNotFound*/ false);
	if (HitCueTag.IsValid())
	{
		FGameplayEffectCue Cue;
		Cue.GameplayCueTags.AddTag(HitCueTag);
		GameplayCues.Add(Cue);
	}
}
