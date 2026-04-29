
#include "SLANS_LightAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"


USLANS_LightAttack::USLANS_LightAttack()
{
}

USLANS_LightAttack::~USLANS_LightAttack()
{
}

void USLANS_LightAttack::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
    AActor* Owner = MeshComp->GetOwner();
    if (!Owner) return;

    FVector TraceLocation = MeshComp->GetSocketLocation(SocketName);
    FRotator TraceRotation = MeshComp->GetSocketRotation(SocketName);

    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(Owner);

    TArray<FHitResult> OutHits;

    // Use BoxTrace for Crescent shapes, or SphereTrace for thrusts
    bool bHit = UKismetSystemLibrary::BoxTraceMulti(
        Owner,
        TraceLocation, TraceLocation, // Start and End are same for a static sweep per tick
        BoxHalfExtents,
        TraceRotation,
        UEngineTypes::ConvertToTraceType(ECC_Pawn), // Or your custom Weapon channel
        false,
        ActorsToIgnore,
        EDrawDebugTrace::ForOneFrame, // Great for debugging your crescent shape!
        OutHits,
        true
    );

    if (bHit)
    {
        UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);
        if (ASC)
        {
            for (const FHitResult& Hit : OutHits)
            {
                FGameplayEventData Payload;
                Payload.Target = Hit.GetActor();
                Payload.Instigator = Owner;

                // Send event to the GA to handle the "Ignore List" and Damage
                ASC->HandleGameplayEvent(EventTag, &Payload);
            }
        }
    }
}
