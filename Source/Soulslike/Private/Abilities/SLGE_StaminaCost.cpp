#include "Abilities/SLGE_StaminaCost.h"

#include "SLCharacterAttributeSet.h"
#include "Weapons/SLWeaponTypes.h"

#include "GameplayEffectTypes.h"

USLGE_StaminaCost::USLGE_StaminaCost()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	// Stamina += SetByCaller("Data.Cost.Stamina"). Callers pass a NEGATIVE value
	// (the helper APIs do the negation) so that the same GE class can also be
	// used to refund stamina if ever needed by passing a positive number.
	FGameplayModifierInfo Mod;
	Mod.Attribute = USLCharacterAttributeSet::GetStaminaAttribute();
	Mod.ModifierOp = EGameplayModOp::Additive;

	FSetByCallerFloat SBC;
	SBC.DataTag = FGameplayTag::RequestGameplayTag(SLCombatTags::SetByCaller_StaminaCost, /*ErrorIfNotFound*/ false);
	Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(SBC);

	Modifiers.Add(Mod);
}
