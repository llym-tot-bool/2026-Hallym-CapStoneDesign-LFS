#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/SLSkillTypes.h"
#include "SLGameplayAbility_Skill.generated.h"

class UGameplayEffect;

/**
 * Base class for player skills. Two player skills can be equipped at a time
 * (Skill1 / Skill2). Skills have a cooldown but no mana cost.
 *
 * Cooldown is applied through the standard GAS path: we override GetCooldownTags
 * and ApplyCooldown so CheckCooldown works transparently. The cooldown duration is
 * driven by CooldownDuration and the per-skill cooldown tag is added as a dynamic
 * granted tag on the cooldown GE, so a single shared CooldownGEClass can service
 * any number of skills.
 *
 * Derive skill Blueprints from this class, set SkillSlot + CooldownDuration + the
 * matching CooldownTag, then grant it via the player's default abilities list.
 */
UCLASS(Abstract, Blueprintable)
class SOULSLIKE_API USLGameplayAbility_Skill : public UGameplayAbility
{
	GENERATED_BODY()

public:
	USLGameplayAbility_Skill();

	/** Which skill slot this ability occupies. Used for slot-based UI / input routing. */
	UFUNCTION(BlueprintPure, Category = "Skill")
	ESLSkillSlot GetSkillSlot() const { return SkillSlot; }

	// --- UGameplayAbility overrides ---------------------------------------
	virtual const FGameplayTagContainer* GetCooldownTags() const override;
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

protected:
	/** Slot this skill is bound to. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	ESLSkillSlot SkillSlot = ESLSkillSlot::None;

	/** Cooldown length in seconds. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Cooldown", meta = (ClampMin = "0.0"))
	float CooldownDuration = 5.0f;

	/**
	 * Tag that identifies "this skill is on cooldown". Granted dynamically by the
	 * cooldown GE. Normally one of SLSkillTags::Cooldown_SkillOne / SkillTwo.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Cooldown")
	FGameplayTag CooldownTag;

	/**
	 * The GE applied when this skill goes on cooldown. Defaults to USLGE_SkillCooldown
	 * but can be overridden in derived classes.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Cooldown")
	TSubclassOf<UGameplayEffect> CooldownGEClass;

private:
	/** Lazily-built, returned from GetCooldownTags so CheckCooldown picks it up. */
	mutable FGameplayTagContainer CachedCooldownTags;
};
