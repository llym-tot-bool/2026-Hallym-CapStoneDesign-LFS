

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "SLDA_MeleeCombat.h"

#include "SLANS_MeleeSweep_Trace.generated.h"

/**
 * 
 */
UCLASS(abstract)
class SOULSLIKE_API USLANS_MeleeSweep_Trace : public UAnimNotifyState
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "TraceState");
    ESL_MeleeSweep_TraceState state_trace;

    UPROPERTY(EditAnywhere, Category = "TraceState");
    ESL_MeleeSweep_TraceState state_none;

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
