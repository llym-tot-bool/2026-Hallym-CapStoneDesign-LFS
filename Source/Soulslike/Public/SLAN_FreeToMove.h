

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"

#include "SLAN_FreeToMove.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class SOULSLIKE_API USLAN_FreeToMove : public UAnimNotify
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "GAS")
	FGameplayTag freeToMove_tag;

public:
	virtual void Notify(
		USkeletalMeshComponent* MeshComp, 
		UAnimSequenceBase* Animation, 
		const FAnimNotifyEventReference& EventReference
	) override;
};
