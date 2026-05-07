

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "SLDA_MeleeCombat.h"

#include "SLAN_MeleeSweep_State.generated.h"

/**
 * 
 */

UCLASS()
class SOULSLIKE_API USLAN_MeleeSweep_State : public UAnimNotify {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "State")
    ESL_MeleeSweep_State state;

    virtual void Notify(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation,
        const FAnimNotifyEventReference& EventReference
    ) override;
};