

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "SLDA_MeleeCombat.h"
#include "SoulslikePlayerState.h"

#include "SLGA_CharDeath.generated.h"


UCLASS()
class SOULSLIKE_API USLGA_CharDeath : public UGameplayAbility
{
	GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, Category = "PlayerMovementRestriction")
    FGameplayTag tag_RootMotion;
    UPROPERTY(EditAnywhere, Category = "PlayerMovementRestriction")
    FGameplayTag tag_IsMoving;

    TObjectPtr<UAbilitySystemComponent> ASC;
    TObjectPtr<ASoulslikePlayerState> SLPS;

protected:

    void setRootMotion();

    virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr,
        FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	
};
