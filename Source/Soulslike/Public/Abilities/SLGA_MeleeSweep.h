

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "SLDA_MeleeCombat.h"

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

UCLASS()
class SOULSLIKE_API USLGA_MeleeSweep : public UGameplayAbility
{
	GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, Category = "PlayerMovementRestriction")
    FGameplayTag tag_RootMotion;
    UPROPERTY(EditAnywhere, Category = "PlayerMovementRestriction")
    FGameplayTag tag_IsMoving;

    UPROPERTY(EditAnywhere, Category = "Collision")
    FVector BoxHalfExtents = FVector(15.f, 15.f, 15.f);

    UPROPERTY(EditAnywhere, Category = "Collision")
    FName socket_weapon_base = "Socket_weapon_base";

    UPROPERTY(EditAnywhere, Category = "Collision")
    FName socket_weapon_tip = "Socket_weapon_tip";

    UPROPERTY(EditAnywhere, Category = "Collision")
    float socket_weapon_length = 50.0f;

    ESL_MeleeSweep_State state;
    ESL_MeleeSweep_TraceState traceState;
    USLAT_MeeleSweep_hit_checker* hitchecker;

    // broadcast to comboGA
    FSL_MeleeSweep_ComboInput delegate_ComboInput;
    FSL_MeleeSweep_Translation delegate_Translation;
    FSL_MeleeSweep_Recovery delegate_Recovery;

protected:

    void setRootMotion();
    void removeRootMotion();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

    UFUNCTION()
    void ChangeState(ESL_MeleeSweep_State newstate);
    UFUNCTION()
    void ChangeTraceState(ESL_MeleeSweep_TraceState newState);
    UFUNCTION()
    void OnCharacteMove();

private:
    void ComboInput();
    void Translate();
    void Recovery();
};
