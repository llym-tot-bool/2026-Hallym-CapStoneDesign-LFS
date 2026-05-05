


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
    payload.EventTag = tag_inputAsCombo;
    payload.Instigator = owner;
    UE_LOG(LogTemp, Display, TEXT("[SL debug] nofity begin with tag = %s"), *tag_inputAsCombo.ToString());
    owner_asc->HandleGameplayEvent(tag_inputAsCombo, &payload);

}

void USLANS_MeleeCombo::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
	const FAnimNotifyEventReference& EventReference)
{
    AActor* owner = MeshComp->GetOwner();
    if (!owner) return;

    UAbilitySystemComponent* owner_asc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(owner);
    if (!owner_asc) return;

    FGameplayEventData payload;
    payload.EventTag = tag_tryActivateCombo;
    payload.Instigator = owner;
    UE_LOG(LogTemp, Display, TEXT("[SL debug] nofity end with tag = %s"), *tag_tryActivateCombo.ToString());
    owner_asc->HandleGameplayEvent(tag_tryActivateCombo, &payload);


	Super::NotifyEnd(MeshComp, Animation, EventReference);
}
