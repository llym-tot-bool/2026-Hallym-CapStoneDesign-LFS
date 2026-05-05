


#include "SLAN_FreeToMove.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void USLAN_FreeToMove::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	AActor* owner = MeshComp->GetOwner();
	if (!owner) return;

	UAbilitySystemComponent* owner_asc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(owner);
	if (!owner_asc) return;

	FGameplayEventData payload;
	payload.EventTag = freeToMove_tag;
	payload.Instigator = owner;
	payload.OptionalObject = Animation;
	owner_asc->HandleGameplayEvent(freeToMove_tag, &payload);
}
