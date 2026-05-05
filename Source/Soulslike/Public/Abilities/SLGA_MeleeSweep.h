

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/Tasks/AbilityTask.h"

#include "SLGA_MeleeSweep.generated.h"

/**
 * 
 */
UCLASS()
class SOULSLIKE_API USLAT_MeeleSweep_hit_checker : public UAbilityTask
{
    GENERATED_BODY()
public:
    // This allows the GA to create the task easily
    static USLAT_MeeleSweep_hit_checker* Create(UGameplayAbility* OwningAbility,
        FName socket_base_name, FName socket_tip_name,
        float trace_length, FVector boxHalfExtents);

    bool IgnoreSelf();

    virtual void TickTask(float DeltaTime) override;
    void EffectOnHit(AActor* hitActor);

private:
    FName socket_base_name;
    FName socket_tip_name;
    float trace_length;
    FVector boxHalfExtents;
    TArray<AActor*> actorsToIgnore;
};

UCLASS(abstract)
class SOULSLIKE_API USLGA_MeleeSweep : public UGameplayAbility
{
	GENERATED_BODY()

protected:

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    FGameplayTag trace_start_tag;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    FGameplayTag trace_end_tag;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    FGameplayTag state_tag;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    FGameplayTag freeToMove_tag;

    UPROPERTY(EditAnywhere, Category = "Collision")
    FVector BoxHalfExtents = FVector(15.f, 15.f, 15.f);

    UPROPERTY(EditAnywhere, Category = "Collision")
    FName socket_weapon_base = "Socket_weapon_base";

    UPROPERTY(EditAnywhere, Category = "Collision")
    FName socket_weapon_tip = "Socket_weapon_tip";

    UPROPERTY(EditAnywhere, Category = "Collision")
    float socket_weapon_length = 50.0f;

    USLAT_MeeleSweep_hit_checker* hitchecker;

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

    UFUNCTION()
    void TraceStart(FGameplayEventData Payload);

    UFUNCTION()
    void TraceEnd(FGameplayEventData Payload);

    UFUNCTION()
    void FreeToMove(FGameplayEventData Payload);
};
