


#include "ANS/SLANS_MeleeSweepHitbox.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"

void USLANS_MeleeSweepHitbox::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    AActor* owner = MeshComp->GetOwner();
    if (!owner) return;

    UAbilitySystemComponent* owner_asc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(owner);
    if (!owner_asc) return;

    FGameplayEventData payload;
    payload.EventTag = tag_traceStart;
    payload.Instigator = owner;
    payload.OptionalObject = this;
    owner_asc->HandleGameplayEvent(tag_traceStart, &payload);
}

void USLANS_MeleeSweepHitbox::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
    AActor* owner = MeshComp->GetOwner();
    if (!owner) return;

    UAbilitySystemComponent* owner_asc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(owner);
    if (!owner_asc) return;

    FGameplayEventData payload;
    payload.EventTag = tag_traceEnd;
    payload.Instigator = owner;
    payload.OptionalObject = this;
    owner_asc->HandleGameplayEvent(tag_traceEnd, &payload);

    Super::NotifyEnd(MeshComp, Animation, EventReference);
}
