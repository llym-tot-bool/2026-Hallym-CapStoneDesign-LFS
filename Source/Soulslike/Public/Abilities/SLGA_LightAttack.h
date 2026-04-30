#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"

#include "SLGA_LightAttack.generated.h"

/**
 * 
 */
UCLASS(abstract)
class SOULSLIKE_API USLGA_LightAttack : public UGameplayAbility
{
	GENERATED_BODY()

protected:
    // This is where we store who we've already hit during THIS specific execution
    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    TArray<AActor*> HitActors;

    // The Tag we listen for from the AnimNotifyState
    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    FGameplayTag HitEventTag;

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    // Function called when the AnimNotifyState detects a hit
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void HandleHitDetection(FGameplayEventData Payload);

    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};
