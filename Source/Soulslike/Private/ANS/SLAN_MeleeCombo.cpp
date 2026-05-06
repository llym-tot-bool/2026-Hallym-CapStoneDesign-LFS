


#include "ANS/SLAN_MeleeCombo.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void USLAN_MeleeCombo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
    const FAnimNotifyEventReference& EventReference)
{
    AActor* owner = MeshComp->GetOwner();
    if (!owner) return;

    UAbilitySystemComponent* owner_asc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(owner);
    if (!owner_asc) return;

    FGameplayEventData payload;
    payload.EventTag = tag_notify;
    payload.Instigator = owner;
    UE_LOG(LogTemp, Display, TEXT("[SL debug] Notify() : tag = %s"), *tag_notify.ToString());
    owner_asc->HandleGameplayEvent(tag_notify, &payload);

    Super::Notify(MeshComp, Animation, EventReference);
}
