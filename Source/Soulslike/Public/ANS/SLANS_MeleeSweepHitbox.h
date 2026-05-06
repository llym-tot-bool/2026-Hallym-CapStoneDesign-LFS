

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"

#include "SLANS_MeleeSweepHitbox.generated.h"

/**
 * 
 */
UCLASS(abstract)
class SOULSLIKE_API USLANS_MeleeSweepHitbox : public UAnimNotifyState
{
	GENERATED_BODY()

public:
    // The Tag to send to the GA (must match HitEventTag in GA)
    UPROPERTY(EditAnywhere, Category = "GAS")
    FGameplayTag tag_traceStart;

    UPROPERTY(EditAnywhere, Category = "GAS")
    FGameplayTag tag_traceEnd;


public:

    virtual void NotifyBegin(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation,
        float TotalDuration,
        const FAnimNotifyEventReference& EventReference
    ) override;

    virtual void NotifyEnd(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation,
        const FAnimNotifyEventReference& EventReference
    ) override;
};
