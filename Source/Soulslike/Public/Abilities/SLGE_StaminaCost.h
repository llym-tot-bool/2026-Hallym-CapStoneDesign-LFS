#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "SLGE_StaminaCost.generated.h"

/**
 * Instant GameplayEffect that subtracts a SetByCaller-driven amount from Stamina.
 *
 * Caller sets `SLCombatTags::SetByCaller_StaminaCost` ("Data.Cost.Stamina") to a
 * POSITIVE cost value; the GE applies it as a negative additive modifier so
 * Stamina decreases. PreAttributeChange clamps Stamina to [0, MaxStamina].
 *
 * This is used both by combat abilities (light attack, dodge) and weapon-driven
 * costs sourced from USLWeaponDataAsset (StaminaCostLight / StaminaCostHeavy).
 */
UCLASS()
class SOULSLIKE_API USLGE_StaminaCost : public UGameplayEffect
{
	GENERATED_BODY()

public:
	USLGE_StaminaCost();
};
