#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "SLGE_StaminaRegen.generated.h"

/**
 * Infinite periodic GameplayEffect that restores Stamina based on the StaminaRegen attribute.
 * Inhibited by State.Stamina.Spending (applied via SLCharacterAttributeSet).
 */
UCLASS()
class SOULSLIKE_API USLGE_StaminaRegen : public UGameplayEffect
{
	GENERATED_BODY()

public:
	USLGE_StaminaRegen();
};
