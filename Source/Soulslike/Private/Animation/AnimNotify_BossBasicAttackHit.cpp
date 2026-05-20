#include "Animation/AnimNotify_BossBasicAttackHit.h"
#include "Boss.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotify_BossBasicAttackHit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		return;
	}

	if (ABoss* Boss = Cast<ABoss>(MeshComp->GetOwner()))
	{
		Boss->OnBasicAttackDamageNotify();
	}
}

FString UAnimNotify_BossBasicAttackHit::GetNotifyName_Implementation() const
{
	return FString(TEXT("Boss Basic Attack Hit"));
}
