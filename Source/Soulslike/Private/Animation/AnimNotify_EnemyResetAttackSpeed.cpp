#include "Animation/AnimNotify_EnemyResetAttackSpeed.h"
#include "enemy_mobs.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotify_EnemyResetAttackSpeed::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		return;
	}

	Aenemy_mobs* EnemyMob = Cast<Aenemy_mobs>(MeshComp->GetOwner());
	if (!EnemyMob)
	{
		return;
	}

	EnemyMob->OnBasicAttackSpeedResetNotify();
}

FString UAnimNotify_EnemyResetAttackSpeed::GetNotifyName_Implementation() const
{
	return FString(TEXT("Enemy Reset Attack Speed"));
}
