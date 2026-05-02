#include "Abilities/SLGameplayAbility_Skill.h"
#include "Abilities/SLGE_SkillCooldown.h"
#include "Abilities/SLSkillTypes.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

USLGameplayAbility_Skill::USLGameplayAbility_Skill()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	CooldownGEClass = USLGE_SkillCooldown::StaticClass();
}

const FGameplayTagContainer* USLGameplayAbility_Skill::GetCooldownTags() const
{
	CachedCooldownTags.Reset();
	if (CooldownTag.IsValid())
	{
		CachedCooldownTags.AddTag(CooldownTag);
	}
	// Fold in whatever the parent class reports (typically empty, but preserve behavior).
	if (const FGameplayTagContainer* ParentTags = Super::GetCooldownTags())
	{
		CachedCooldownTags.AppendTags(*ParentTags);
	}
	return &CachedCooldownTags;
}

void USLGameplayAbility_Skill::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (!CooldownGEClass || CooldownDuration <= 0.f || !CooldownTag.IsValid())
	{
		return;
	}

	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(Handle, ActorInfo, ActivationInfo, CooldownGEClass);
	if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid())
	{
		return;
	}

	// Tag the GE so it identifies this specific skill's cooldown — allows a single
	// shared cooldown GE class to service every skill slot.
	SpecHandle.Data->DynamicGrantedTags.AddTag(CooldownTag);

	SpecHandle.Data->SetSetByCallerMagnitude(
		FGameplayTag::RequestGameplayTag(SLSkillTags::SetByCaller_CooldownDuration, /*ErrorIfNotFound*/ false),
		CooldownDuration);

	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
}
