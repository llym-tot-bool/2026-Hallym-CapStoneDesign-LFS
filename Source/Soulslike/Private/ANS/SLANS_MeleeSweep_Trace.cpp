


#include "ANS/SLANS_MeleeSweep_Trace.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SoulslikePlayerState.h"

void USLANS_MeleeSweep_Trace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    AActor* Owner = MeshComp->GetOwner();
    if (!Owner) return;

    APawn* Pawn = Cast<APawn>(Owner);
    if (!Pawn) return;

    APlayerState* PS = Pawn->GetPlayerState();
    if (!PS) return;

    ASoulslikePlayerState* SLPS = Cast<ASoulslikePlayerState>(PS);
    if (!SLPS) return;
    if (!SLPS->delegate_MeleeSweep_TraceState.IsBound()) return;
    
    SLPS->delegate_MeleeSweep_TraceState.Broadcast(state_trace);
}

void USLANS_MeleeSweep_Trace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
    AActor* Owner = MeshComp->GetOwner();
    if (!Owner) return;

    APawn* Pawn = Cast<APawn>(Owner);
    if (!Pawn) return;

    APlayerState* PS = Pawn->GetPlayerState();
    if (!PS) return;

    ASoulslikePlayerState* SLPS = Cast<ASoulslikePlayerState>(PS);
    if (!SLPS) return;

    SLPS->delegate_MeleeSweep_TraceState.Broadcast(state_none);

    Super::NotifyEnd(MeshComp, Animation, EventReference);
}
