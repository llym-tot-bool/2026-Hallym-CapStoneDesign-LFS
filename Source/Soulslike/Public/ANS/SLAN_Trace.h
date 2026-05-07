

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "SLDA_MeleeCombat.h"

#include "SLAN_Trace.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class SOULSLIKE_API USLAN_Trace : public UAnimNotify
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "TraceState")
	ESL_MeleeSweep_TraceState traceState;

public:
	virtual void Notify(
		USkeletalMeshComponent* MeshComp, 
		UAnimSequenceBase* Animation, 
		const FAnimNotifyEventReference& EventReference
	) override;
};
