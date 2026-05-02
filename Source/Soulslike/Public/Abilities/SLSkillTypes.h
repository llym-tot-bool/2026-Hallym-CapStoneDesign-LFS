#pragma once

#include "CoreMinimal.h"
#include "SLSkillTypes.generated.h"

/** Identifies which player skill slot an ability occupies. */
UENUM(BlueprintType)
enum class ESLSkillSlot : uint8
{
	None	UMETA(DisplayName = "None"),
	SkillOne	UMETA(DisplayName = "Skill 1"),
	SkillTwo	UMETA(DisplayName = "Skill 2")
};

/**
 * Centralizes the gameplay tag names used by the skill system so ability code and
 * input code don't drift out of sync. These are resolved via FGameplayTag::RequestGameplayTag
 * at call time — the tags themselves must exist in the project's tag table.
 */
namespace SLSkillTags
{
	// Activation tags — character input translates to TryActivateAbilitiesByTag with these.
	static const FName Activate_SkillOne = TEXT("PlayerAbility.Skill.One");
	static const FName Activate_SkillTwo = TEXT("PlayerAbility.Skill.Two");

	// Cooldown tags — granted by the cooldown GE while the skill is on cooldown.
	static const FName Cooldown_SkillOne = TEXT("Cooldown.Skill.One");
	static const FName Cooldown_SkillTwo = TEXT("Cooldown.Skill.Two");

	// SetByCaller magnitude tag used by UGE_SkillCooldown for its duration.
	static const FName SetByCaller_CooldownDuration = TEXT("Data.Cooldown.Duration");
}
