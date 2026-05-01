
#include "Abilities/SLGA_LightAttack.h"
#include "SoulslikeCharacter.h"
#include "Kismet/KismetSystemLibrary.h"


void USLGA_LightAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	HitActors.Empty(); // Safety clear

    AActor* avatar = GetAvatarActorFromActorInfo();
    if (!avatar) {
        UE_LOG(LogTemp, Display, TEXT("SLGA_LightAttack ActivateAbility() : no avatar"));
        return;
    }

    ASoulslikeCharacter* character = Cast<ASoulslikeCharacter>(avatar);
    if (!character) {
        UE_LOG(LogTemp, Display, TEXT("SLGA_LightAttack ActivateAbility() : cast avatar to character failed"));
        return;
    }

    USkeletalMeshComponent* mesh_comp = character->GetMesh();
    if (!mesh_comp) {
        UE_LOG(LogTemp, Display, TEXT("SLGA_LightAttack ActivateAbility() : get mesh failed"));
        return;
    }

    FVector TraceLocation = mesh_comp->GetSocketLocation(SocketName);
    FRotator TraceRotation = mesh_comp->GetSocketRotation(SocketName);

    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(avatar);

    TArray<FHitResult> OutHits;

    // Use BoxTrace for Crescent shapes, or SphereTrace for thrusts
    bool bHit = UKismetSystemLibrary::BoxTraceMulti(
        avatar,
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

    if (bHit) {
        for (const FHitResult& hitresult : OutHits) {
            UE_LOG(LogTemp, Display, TEXT("SLGA_LightAttack ActivateAbility() : hit"))
        }
    }

    UE_LOG(LogTemp, Display, TEXT("SLGA_LightAttack ActivateAbility() : end"));
}

void USLGA_LightAttack::HandleHitDetection(FGameplayEventData Payload)
{
    UE_LOG(LogTemp, Display, TEXT("HandleHitDetection called"))

    AActor* Target = const_cast<AActor*>(Payload.Target.Get());

    if (Target && !HitActors.Contains(Target))
    {
        HitActors.Add(Target);
        GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("LightAttack Hit Landed!"));

        // Apply Gameplay Effects (Damage, Knockback) here
        // GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectToTarget(...)
    }
}

void USLGA_LightAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    HitActors.Empty(); // Clean up memory
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
