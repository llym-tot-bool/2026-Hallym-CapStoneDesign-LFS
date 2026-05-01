


#include "Abilities/SLGA_MeleeSweep.h"
#include "SoulslikeCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

USLAT_MeeleSweep_hit_checker* USLAT_MeeleSweep_hit_checker::Create(UGameplayAbility* OwningAbility,
    FName socket_start_name, FName socket_end_name,
    float trace_length, FVector boxHalfExtents)
{
    USLAT_MeeleSweep_hit_checker* hit_checker = NewAbilityTask<USLAT_MeeleSweep_hit_checker>(OwningAbility);
    hit_checker->socket_base_name = socket_start_name;
    hit_checker->socket_tip_name = socket_end_name;
    hit_checker->trace_length = trace_length;
    hit_checker->boxHalfExtents = boxHalfExtents;

    hit_checker->actorsToIgnore.Empty();
    if (!hit_checker->IgnoreSelf()) return nullptr;

    hit_checker->bTickingTask = true;
    return hit_checker;
}

bool USLAT_MeeleSweep_hit_checker::IgnoreSelf()
{
    AActor* avatar = GetAvatarActor();
    if (!avatar) return false;
    actorsToIgnore.Add(avatar);
    return true;
}

void USLAT_MeeleSweep_hit_checker::TickTask(float DeltaTime)
{
    Super::TickTask(DeltaTime);

    AActor* avatar = GetAvatarActor();
    if (!avatar) return;

    ASoulslikeCharacter* character = Cast<ASoulslikeCharacter>(avatar);
    if (!character) return;

    USkeletalMeshComponent* mesh_comp = character->GetMesh();
    if (!mesh_comp) return;

    FVector trace_start = mesh_comp->GetSocketLocation(socket_base_name);
    FRotator trace_rotation = mesh_comp->GetSocketRotation(socket_base_name);
    FVector socket_tip_loc = mesh_comp->GetSocketLocation(socket_tip_name);

    FVector trace_dir = (socket_tip_loc - trace_start).GetUnsafeNormal();
    FVector trace_end = trace_start + (trace_dir * trace_length);

    TArray<FHitResult> OutHits;

    // Use BoxTrace for Crescent shapes, or SphereTrace for thrusts
    bool bHit = UKismetSystemLibrary::BoxTraceMulti(
        avatar,
        trace_start, trace_end, // Start and End are same for a static sweep per tick
        boxHalfExtents,
        trace_rotation,
        UEngineTypes::ConvertToTraceType(ECC_Pawn), // Or your custom Weapon channel
        false,
        actorsToIgnore,
        EDrawDebugTrace::ForOneFrame, // Great for debugging your crescent shape!
        OutHits,
        true
    );

    if (bHit) {
        for (const FHitResult& hitresult : OutHits) {
            AActor* hitActor = hitresult.GetActor();
            if (!hitActor) continue;
            if (actorsToIgnore.Contains(hitActor)) continue;

            actorsToIgnore.Add(hitActor);
            EffectOnHit(hitActor);
        }
    }
}

void USLGA_MeleeSweep::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    UE_LOG(LogTemp, Display, TEXT("SLGA_LightAttack ActivateAbility() : called")); // debug

    // 1. Wait for the 'Start' event from your ANS
    UAbilityTask_WaitGameplayEvent* WaitStart = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, trace_start_tag);
    if (WaitStart) {
        WaitStart->EventReceived.AddDynamic(this, &USLGA_MeleeSweep::TraceStart);
        WaitStart->ReadyForActivation();
    }
    else {
        UE_LOG(LogTemp, Display, TEXT("SLGA_LightAttack ActivateAbility() : trace start event listener failed"));
    }

    // 2. Wait for the 'End' event to stop everything
    UAbilityTask_WaitGameplayEvent* WaitEnd = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, trace_end_tag);
    if (WaitEnd) {
        WaitEnd->EventReceived.AddDynamic(this, &USLGA_MeleeSweep::TraceEnd);
        WaitEnd->ReadyForActivation();
    }
    else {
        UE_LOG(LogTemp, Display, TEXT("SLGA_LightAttack ActivateAbility() : tarce end event listener failed"));
    }

    hitchecker = USLAT_MeeleSweep_hit_checker::Create(
        this,
        socket_weapon_base, socket_weapon_tip, socket_weapon_length,
        BoxHalfExtents);
}

void USLGA_MeleeSweep::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    hitchecker->EndTask();
    UE_LOG(LogTemp, Display, TEXT("SLGA_LightAttack EndAbility() : called")); // debug
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


void USLGA_MeleeSweep::TraceStart(FGameplayEventData Payload)
{
    hitchecker->ReadyForActivation();
}

void USLGA_MeleeSweep::TraceEnd(FGameplayEventData Payload)
{
    hitchecker->EndTask();
}

void USLAT_MeeleSweep_hit_checker::EffectOnHit(AActor* hitActor)
{
    FString actorName = hitActor->GetName();
    UE_LOG(LogTemp, Display, TEXT("SLAT_Melee EffectOnHit() : hit = %s"), *actorName);
}