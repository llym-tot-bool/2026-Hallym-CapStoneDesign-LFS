#include "Animation/AnimNotify_BossMoveBehindTarget.h"
#include "Boss.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotify_BossMoveBehindTarget::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		return;
	}

	if (ABoss* Boss = Cast<ABoss>(MeshComp->GetOwner()))
	{
		Boss->OnMoveBehindTargetNotify();
	}
}

FString UAnimNotify_BossMoveBehindTarget::GetNotifyName_Implementation() const
{
	return FString(TEXT("Boss Move Behind Target"));
}
