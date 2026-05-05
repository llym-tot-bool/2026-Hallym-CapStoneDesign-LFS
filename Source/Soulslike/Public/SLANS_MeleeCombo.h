

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"

#include "SLANS_MeleeCombo.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class SOULSLIKE_API USLANS_MeleeCombo : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "GAS")
    FGameplayTag comboInputStart_tag;

    UPROPERTY(EditAnywhere, Category = "GAS")
    FGameplayTag comboStart_tag;

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
