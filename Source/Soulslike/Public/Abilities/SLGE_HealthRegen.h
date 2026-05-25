#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "SLGE_HealthRegen.generated.h"

/**
 * Infinite periodic GameplayEffect that restores Health based on the HealthRegen attribute.
 * Inhibited by State.RecentlyDamaged (applied via SLCharacterAttributeSet).
 */
UCLASS()
class SOULSLIKE_API USLGE_HealthRegen : public UGameplayEffect
{
	GENERATED_BODY()

public:
	USLGE_HealthRegen();
};
