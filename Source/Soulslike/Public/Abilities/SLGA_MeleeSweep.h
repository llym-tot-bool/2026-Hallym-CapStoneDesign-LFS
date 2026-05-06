

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

    UPROPERTY(EditDefaultsOnly, Category = "Combat|Trace")
    FGameplayTag tag_traceStart;

    UPROPERTY(EditDefaultsOnly, Category = "Combat|Trace")
    FGameplayTag tag_traceEnd;

    UPROPERTY(EditDefaultsOnly, Category = "Combat|Movement_restriction")
    FGameplayTag tag_stateAttacking;
    int cnt_tag_stateAttacking;

    UPROPERTY(EditDefaultsOnly, Category = "Combat|Movement_restriction")
    FGameplayTag tag_freeToMove;

    UPROPERTY(EditDefaultsOnly, Category = "Combat|Movement_restriction")
    FGameplayTag tag_tryingToMove;

    UPROPERTY(EditDefaultsOnly, Category = "Combat|Combo")
    FGameplayTag tag_inputAsCombo;

    UPROPERTY(EditDefaultsOnly, Category = "Combat|Combo")
    FGameplayTag tag_comboGrant;

    UPROPERTY(EditDefaultsOnly, Category = "Combat|Combo")
    FGameplayTag tag_tryActivateCombo;

    UPROPERTY(EditDefaultsOnly, Category = "Combat|Combo")
    TSubclassOf<UGameplayAbility> comboAbility;

    UPROPERTY(EditDefaultsOnly, Category = "Combat|Combo")
    FGameplayTag tag_comboAbility;

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

    UFUNCTION()
    void InputAsCombo(FGameplayEventData Payload);

    UFUNCTION()
    void TryActivateCombo(FGameplayEventData Payload);
};
