
#include "Abilities/SLGA_LightAttack.h"
#include "SoulslikeCharacter.h"
#include "Kismet/KismetSystemLibrary.h"


void USLGA_LightAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    UE_LOG(LogTemp, Display, TEXT("SLGA_LightAttack ActivateAbility() : called")); // debug

    hitchecker = USLAT_LightAttack_hit_checker::Create(
        this, 
        socket_weapon_base, socket_weapon_tip, socket_weapon_length,
        BoxHalfExtents);

    hitchecker->ReadyForActivation();
}

void USLGA_LightAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    hitchecker->EndTask();
    UE_LOG(LogTemp, Display, TEXT("SLGA_LightAttack EndAbility() : called")); // debug
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

USLAT_LightAttack_hit_checker* USLAT_LightAttack_hit_checker::Create(UGameplayAbility* OwningAbility, 
    FName socket_start_name, FName socket_end_name, 
    float trace_length, FVector boxHalfExtents)
{
    USLAT_LightAttack_hit_checker* hit_checker = NewAbilityTask<USLAT_LightAttack_hit_checker>(OwningAbility);
    hit_checker->socket_base_name = socket_start_name;
    hit_checker->socket_tip_name = socket_end_name;
    hit_checker->trace_length = trace_length;
    hit_checker->boxHalfExtents = boxHalfExtents;
    
    hit_checker->actorsToIgnore.Empty();
    if (!hit_checker->IgnoreSelf()) return nullptr;
    
    hit_checker->bTickingTask = true;
    return hit_checker;
}

bool USLAT_LightAttack_hit_checker::IgnoreSelf()
{
    AActor* avatar = GetAvatarActor();
    if (!avatar) return false;
    actorsToIgnore.Add(avatar);
    return true;
}

void USLAT_LightAttack_hit_checker::TickTask(float DeltaTime)
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

void USLAT_LightAttack_hit_checker::EffectOnHit(AActor* hitActor)
{
    FString actorName = hitActor->GetName();
    UE_LOG(LogTemp, Display, TEXT("SLAT_LightAttack EffectOnHit() : hit = %s"), *actorName);
}
