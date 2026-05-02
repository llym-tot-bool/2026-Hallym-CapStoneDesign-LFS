#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "SLGE_StaminaRegen.generated.h"

/**
 * Infinite-duration periodic GameplayEffect that restores Stamina over time.
 *
 * Regen is paused (via OngoingTagRequirements.IgnoreTags) whenever the target
 * carries State.Stamina.Spending. The attribute set sets that tag every time
 * Stamina is decreased and clears it on a timer, so regen halts the moment
 * the player spends stamina and resumes a short delay after they stop.
 */
UCLASS()
class SOULSLIKE_API USLGE_StaminaRegen : public UGameplayEffect
{
	GENERATED_BODY()

public:
	USLGE_StaminaRegen();
};
