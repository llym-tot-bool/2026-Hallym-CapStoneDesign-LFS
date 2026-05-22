

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "SLDA_MeleeCombat.h"
#include "SoulslikePlayerState.h"
#include "NiagaraSystem.h"

#include "SLGA_MeleeSweep.generated.h"

/**
 * 
 */

UCLASS()
class SOULSLIKE_API USLAT_Meele_hit_checker : public UAbilityTask
{
    GENERATED_BODY()
public:
    // This allows the GA to create the task easily
    static USLAT_Meele_hit_checker* Create(UGameplayAbility* OwningAbility,
        FName socket_base_name, FName socket_tip_name,
        float trace_length, FVector boxHalfExtents,
        USoundBase* InHitSound, UNiagaraSystem* vfx_onhit, 
        TSubclassOf<UGameplayEffect> OnHitGE, float BaseDamageValue);

    bool IgnoreSelf();
    void SetIsScanning(const bool value);

    void ChangeTraceSpec(FName new_base_name, FName new_tip_name, float new_trace_length, FVector new_boxHalfExtents);
    void ChangeHitSound(USoundBase* new_sound);
    void ChangeVFX(UNiagaraSystem* new_vfx);
    void ChangeGE(TSubclassOf<UGameplayEffect> new_OnHitGE, float new_baseDamage);
    void FlushIgnoreList();

    virtual void TickTask(float DeltaTime) override;
    void EffectOnHit(AActor* hitActor, UAbilitySystemComponent* hitASC, const FVector& HitLocation);

private:
    FName socket_base_name;
    FName socket_tip_name;
    float trace_length;
    FVector boxHalfExtents;
    TArray<AActor*> actorsToIgnore;
    bool isScanning;

    TObjectPtr<USoundBase> HitSound;
    TObjectPtr<UNiagaraSystem> VFX_onhit;
    TSubclassOf<UGameplayEffect> OnHitGE;
    float BaseDamageValue = 0;
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

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack", meta = (ClampMin = "0.0"))
    float StaminaCost = 25.f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack|GAS")
    TSubclassOf<UGameplayEffect> StaminaCostGEClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|Sound")
    TObjectPtr<USoundBase> HitSound;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|VFX")
    TObjectPtr<UNiagaraSystem> VFX_onhit;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|GameplayEffect")
    TSubclassOf<UGameplayEffect> OnHitGE;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack|GameplayEffect")
    float BaseDamageValue = 0;

    ESL_Melee_State state;
    ESL_Melee_TraceState traceState;
    USLAT_Meele_hit_checker* hitchecker;

    TObjectPtr<UAbilitySystemComponent> ASC;
    TObjectPtr<ASoulslikePlayerState> SLPS;
    bool bInterruptAsCombo = false;

public:
    // broadcast to combo component
    FSLDLG_AN_ComboInput delegate_ComboInput;
    FSLDLG_AN_Translation delegate_Translation;
    FSLDLG_AN_Recovery delegate_Recovery;

public:
    void InterruptAsCombo();
    void InterruptAsCancel();

protected:

    void setRootMotion();
    void removeRootMotion();
    
    virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr,
        FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

    UFUNCTION()
    void ChangeState(ESL_Melee_State newstate);
    UFUNCTION()
    void ChangeTraceState(ESL_Melee_TraceState newState);
    UFUNCTION()
    void OnCharacteMove();

private:

    // deligate binding function
    void ComboInput();
    void Translation();
    void Recovery();
};
