
#include "Abilities/SLGA_LightAttack.h"

void USLGA_LightAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	HitActors.Empty(); // Safety clear
}

void USLGA_LightAttack::HandleHitDetection(FGameplayEventData Payload)
{
    UE_LOG(LogTemp, Display, TEXT("HandleHitDetection called"))

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
    HitActors.Empty(); // Clean up memory
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
