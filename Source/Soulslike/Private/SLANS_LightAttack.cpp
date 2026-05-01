
#include "SLANS_LightAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"


void USLANS_LightAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
    float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    // debug start
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("SLANS_LightAttack begin"));
    // debug end

    AActor* owner = MeshComp->GetOwner();
    if (!owner) return;

    UAbilitySystemComponent* owner_asc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(owner);
    if (!owner_asc) return;

    owner_asc->HandleGameplayEvent(EventTagStart, nullptr);

}

void USLANS_LightAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
    const FAnimNotifyEventReference& EventReference)
{
    // debug start
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("SLAND_LightAttack end."));
    // debug end

    AActor* owner = MeshComp->GetOwner();
    if (!owner) return;

    UAbilitySystemComponent* owner_asc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(owner);
    if (!owner_asc) return;

    owner_asc->HandleGameplayEvent(EventTagEnd, nullptr);
}