#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SLGameplayAbility_Dodge.generated.h"

class UAnimMontage;
class UGameplayEffect;

/**
 * Dodge roll ability. Soulslike-standard i-frames + stamina cost.
 *
 * On activation:
 *   1. Apply a stamina-cost GE (USLGE_StaminaCost) sized by StaminaCost.
 *   2. Add State.Dodging + State.Invulnerable as ability-granted tags so
 *      attack abilities that block on State.Dodging cancel themselves and
 *      damage GEs that check State.Invulnerable short-circuit.
 *   3. Play DodgeMontage on the owning character; the ability stays active
 *      until the montage finishes (or the ability is cancelled).
 *
 * Designers derive a BP from this class and assign DodgeMontage. The base
 * implementation in C++ handles the cost / tags / lifecycle so behavior is
 * consistent across weapon classes.
 */
UCLASS(Abstract, Blueprintable)
class SOULSLIKE_API USLGameplayAbility_Dodge : public UGameplayAbility
{
	GENERATED_BODY()

public:
	USLGameplayAbility_Dodge();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dodge", meta = (ClampMin = "0.0"))
	float StaminaCost = 25.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dodge")
	TObjectPtr<UAnimMontage> DodgeMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dodge|GAS")
	TSubclassOf<UGameplayEffect> StaminaCostGEClass;

	UFUNCTION()
	void OnDodgeMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};
