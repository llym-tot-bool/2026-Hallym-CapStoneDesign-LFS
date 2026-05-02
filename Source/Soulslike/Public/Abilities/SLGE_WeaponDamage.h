#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "SLGE_WeaponDamage.generated.h"

/**
 * Instant damage GameplayEffect applied by weapons (and damaging abilities) on hit.
 *
 * The damage magnitude is driven by a SetByCaller value tagged
 * `SLCombatTags::SetByCaller_DamageBase` ("Data.Damage.Base"). The caller —
 * usually ASLWeaponBase::ApplyDamageToTarget — packs the final damage number
 * into the spec; this GE then writes it into the Damage meta-attribute, which
 * USLCharacterAttributeSet::PostGameplayEffectExecute consumes and subtracts
 * from Health.
 *
 * Designers can subclass this in Blueprints to attach extra modifiers (e.g.
 * granted hit-stagger tags, status effect application) without changing the
 * core pipeline.
 */
UCLASS()
class SOULSLIKE_API USLGE_WeaponDamage : public UGameplayEffect
{
	GENERATED_BODY()

public:
	USLGE_WeaponDamage();
};
