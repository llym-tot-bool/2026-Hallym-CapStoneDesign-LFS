

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "SLDA_MeleeCombat.h"
#include "SoulslikePlayerState.h"
#include "SLGA_MeleeSweep.h"

#include "SLGA_MeleeMultiMontage.generated.h"

UCLASS()
class SOULSLIKE_API USLGA_MeleeMultiMontage : public UGameplayAbility
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

    ESL_Melee_State state;
    ESL_Melee_TraceState traceState;
    USLAT_Meele_hit_checker* hitchecker;

    TObjectPtr<UAbilitySystemComponent> ASC;
    TObjectPtr<ASoulslikePlayerState> SLPS;
    TObjectPtr<UAnimInstance> AnimInst;

    UPROPERTY(EditAnywhere, Category = Montage)
    TArray<TObjectPtr<UAnimMontage>> Montage_list;
    
    int currentMontageIdx;
    int lastMontageIdx;

    TObjectPtr<UAbilityTask_PlayMontageAndWait> currentMontageTask;

public:
    // broadcast to combo component
    FSLDLG_AN_ComboInput delegate_ComboInput;
    FSLDLG_AN_Translation delegate_Translation;
    FSLDLG_AN_Recovery delegate_Recovery;

public:
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

    void PlayFirstMontage();
    void PlayNextMontage();

    void ObserveMontage();
    void ObserveQuit();

    UFUNCTION()
    void ChangeState(ESL_Melee_State newstate);
    UFUNCTION()
    void ChangeTraceState(ESL_Melee_TraceState newState);
    UFUNCTION()
    void OnCharacteMove();

    UFUNCTION()
    void OnMontageFinished();

private:

    // deligate binding function
    void Recovery();
	
};
