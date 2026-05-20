#include "Animation/AnimNotify_BossResetAttackSpeed.h"
#include "Boss.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotify_BossResetAttackSpeed::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		return;
	}

	ABoss* Boss = Cast<ABoss>(MeshComp->GetOwner());
	if (!Boss)
	{
		return;
	}

	Boss->OnBasicAttackSpeedResetNotify();
}

FString UAnimNotify_BossResetAttackSpeed::GetNotifyName_Implementation() const
{
	return FString(TEXT("Boss Reset Attack Speed"));
}
