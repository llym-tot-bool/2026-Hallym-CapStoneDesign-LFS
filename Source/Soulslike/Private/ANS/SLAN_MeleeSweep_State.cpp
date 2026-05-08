


#include "ANS/SLAN_MeleeSweep_State.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "SoulslikePlayerState.h"

void USLAN_MeleeSweep_State::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
    const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    AActor* Owner = MeshComp->GetOwner();
    if (!Owner) return;

    APawn* Pawn = Cast<APawn>(Owner);
    if (!Pawn) return;

    APlayerState* PS = Pawn->GetPlayerState();
    if (!PS) return;

    ASoulslikePlayerState* SLPS = Cast<ASoulslikePlayerState>(PS);
    if (!SLPS) return;

    SLPS->delegate_MeleeSweep_State.Broadcast(state);
}
