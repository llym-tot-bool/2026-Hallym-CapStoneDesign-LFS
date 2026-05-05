


#include "SLANS_MeleeCombo.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void USLANS_MeleeCombo::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    AActor* owner = MeshComp->GetOwner();
    if (!owner) return;

    UAbilitySystemComponent* owner_asc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(owner);
    if (!owner_asc) return;

    FGameplayEventData payload;
    payload.EventTag = comboInputStart_tag;
    payload.Instigator = owner;
    owner_asc->HandleGameplayEvent(comboInputStart_tag, &payload);
}

void USLANS_MeleeCombo::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
	const FAnimNotifyEventReference& EventReference)
{
    AActor* owner = MeshComp->GetOwner();
    if (!owner) return;

    UAbilitySystemComponent* owner_asc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(owner);
    if (!owner_asc) return;

    FGameplayEventData payload;
    payload.EventTag = comboStart_tag;
    payload.Instigator = owner;
    owner_asc->HandleGameplayEvent(comboStart_tag, &payload);

	Super::NotifyEnd(MeshComp, Animation, EventReference);
}
