#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_BossMoveBehindTarget.generated.h"

UCLASS(meta = (DisplayName = "Boss Move Behind Target"))
class SOULSLIKE_API UAnimNotify_BossMoveBehindTarget : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;
};
