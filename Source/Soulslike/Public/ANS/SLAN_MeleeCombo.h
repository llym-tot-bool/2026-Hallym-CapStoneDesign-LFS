

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"

#include "SLAN_MeleeCombo.generated.h"

/**
 * 
 */

UCLASS()
class SOULSLIKE_API USLAN_MeleeCombo : public UAnimNotify {
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "GAS")
    FGameplayTag tag_notify;

    virtual void Notify(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation,
        const FAnimNotifyEventReference& EventReference
    ) override;
};