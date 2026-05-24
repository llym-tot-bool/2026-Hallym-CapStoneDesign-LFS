


#include "Abilities/SLGA_CharDeath.h"

#include "Soulslike.h"

void USLGA_CharDeath::setRootMotion()
{
    TObjectPtr<UAbilitySystemComponent> asc = GetAbilitySystemComponentFromActorInfo();
    if (!asc) return;

    if (!asc->HasMatchingGameplayTag(tag_RootMotion)) {
        asc->AddLooseGameplayTag(tag_RootMotion);
    }

    if (asc->HasMatchingGameplayTag(tag_IsMoving)) {
        asc->RemoveLooseGameplayTag(tag_IsMoving);
    }
}

bool USLGA_CharDeath::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return true;
}

void USLGA_CharDeath::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    setRootMotion();

    // delegate binding
    ASC = GetAbilitySystemComponentFromActorInfo(); ensureOrQuit(ASC);
    SLPS = Cast<ASoulslikePlayerState>(ASC->GetOwner()); ensureOrQuit(SLPS);;
}

void USLGA_CharDeath::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
