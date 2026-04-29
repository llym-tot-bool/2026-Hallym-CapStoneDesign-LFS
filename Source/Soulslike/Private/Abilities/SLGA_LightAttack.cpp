
#include "Abilities/SLGA_LightAttack.h"


USLGA_LightAttack::USLGA_LightAttack()
{
}

USLGA_LightAttack::~USLGA_LightAttack()
{
}

void USLGA_LightAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	HitActors.Empty(); // Safety clear
}

void USLGA_LightAttack::HandleHitDetection(FGameplayEventData Payload)
{
    AActor* Target = const_cast<AActor*>(Payload.Target.Get());

    if (Target && !HitActors.Contains(Target))
    {
        HitActors.Add(Target);
        GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("LightAttack Hit Landed!"));

        // Apply Gameplay Effects (Damage, Knockback) here
        // GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectToTarget(...)
    }
}

void USLGA_LightAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
    HitActors.Empty(); // Clean up memory
}
