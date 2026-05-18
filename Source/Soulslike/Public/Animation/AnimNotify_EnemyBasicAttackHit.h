#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_EnemyBasicAttackHit.generated.h"

/**
 * Triggers enemy basic attack damage exactly at montage notify timing.
 */
UCLASS(meta = (DisplayName = "Enemy Basic Attack Hit"))
class SOULSLIKE_API UAnimNotify_EnemyBasicAttackHit : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;
};
