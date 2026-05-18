#include "Animation/AnimNotify_EnemyBasicAttackHit.h"
#include "enemy_mobs.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotify_EnemyBasicAttackHit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
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

	EnemyMob->OnBasicAttackDamageNotify();
}

FString UAnimNotify_EnemyBasicAttackHit::GetNotifyName_Implementation() const
{
	return FString(TEXT("Enemy Basic Attack Hit"));
}
