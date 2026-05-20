#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_BossResetAttackSpeed.generated.h"

/**
 * Resets boss attack montage play rate back to 1.0.
 */
UCLASS(meta = (DisplayName = "Boss Reset Attack Speed"))
class SOULSLIKE_API UAnimNotify_BossResetAttackSpeed : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;
};
