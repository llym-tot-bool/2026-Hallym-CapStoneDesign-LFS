#pragma once

#include "CoreMinimal.h"
#include "SLWeaponTypes.generated.h"

/** Weapon category — drives animations, stance, and which ability set gets granted. */
UENUM(BlueprintType)
enum class ESLWeaponCategory : uint8
{
	None		UMETA(DisplayName = "None"),
	Straight	UMETA(DisplayName = "Straight Sword"),
	Great		UMETA(DisplayName = "Greatsword"),
	Dagger		UMETA(DisplayName = "Dagger"),
	Axe			UMETA(DisplayName = "Axe"),
	Spear		UMETA(DisplayName = "Spear"),
	Bow			UMETA(DisplayName = "Bow"),
	Staff		UMETA(DisplayName = "Staff"),
	Shield		UMETA(DisplayName = "Shield")
};

/**
 * Centralized gameplay tag names for the combat system. Resolved via
 * FGameplayTag::RequestGameplayTag at call time — the tags must exist in the
 * project's tag table (DefaultGameplayTags.ini or equivalent).
 */
namespace SLCombatTags
{
	// SetByCaller magnitude tag used by USLGE_WeaponDamage. The caller (weapon /
	// ability) sets this to the final damage value before applying the spec.
	static const FName SetByCaller_DamageBase = TEXT("Data.Damage.Base");

	// SetByCaller magnitude tag for poise damage. Same pattern as Damage.
	static const FName SetByCaller_PoiseDamage = TEXT("Data.Damage.Poise");

	// SetByCaller magnitude tag for stamina cost. USLGE_StaminaCost reads this and
	// subtracts it from the Stamina attribute.
	static const FName SetByCaller_StaminaCost = TEXT("Data.Cost.Stamina");

	// State tags applied during dodge / invulnerability windows. Other abilities
	// query / block on these (e.g. attack abilities cancelled by State.Dodging,
	// damage GEs short-circuit on State.Invulnerable).
	static const FName State_Dodging = TEXT("State.Dodging");
	static const FName State_Invulnerable = TEXT("State.Invulnerable");

	// Activation tag for the dodge ability — character input maps to this.
	static const FName Activate_Dodge = TEXT("PlayerAbility.Dodge");

	// Applied as a loose gameplay tag on the actor's ASC when Health hits 0.
	// Movement / input / abilities short-circuit on this.
	static const FName State_Dead = TEXT("State.Dead");

	// Loose tag set whenever Stamina is consumed. USLGE_StaminaRegen lists this
	// in its OngoingTagRequirements.IgnoreTags so periodic regen ticks pause
	// while the tag is present. The attribute set clears the tag via timer
	// after a short delay, which is what makes regen "kick in" after a pause.
	static const FName State_StaminaSpending = TEXT("State.Stamina.Spending");

	// Sent as a gameplay event the moment a lethal hit lands. Listeners are
	// expected on the dying actor — death montage, ragdoll, AI cleanup, etc.
	static const FName Event_Death = TEXT("Event.Combat.Death");

	// GameplayCue tag triggered on every damage application. BP cue handlers
	// implement the visual / audio / hit-flinch montage response.
	static const FName Cue_Hit = TEXT("GameplayCue.Combat.Hit");
}
