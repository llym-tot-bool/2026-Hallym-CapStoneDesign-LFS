#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "SLGE_SkillCooldown.generated.h"

/**
 * Shared cooldown GameplayEffect for skills.
 *
 * Duration is driven by a SetByCaller magnitude (tag "Data.Cooldown.Duration"), set by
 * USLGameplayAbility_Skill::ApplyCooldown. The per-skill cooldown identifier tag
 * (e.g. "Cooldown.Skill.One") is added via DynamicGrantedTags from the ability, so
 * one GE class can service every skill.
 */
UCLASS()
class SOULSLIKE_API USLGE_SkillCooldown : public UGameplayEffect
{
	GENERATED_BODY()

public:
	USLGE_SkillCooldown();
};
