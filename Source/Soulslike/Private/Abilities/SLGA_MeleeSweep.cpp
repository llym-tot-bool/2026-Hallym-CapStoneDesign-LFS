


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
    FString myName = this->GetName();
    UE_LOG(LogTemp, Display, TEXT("[SL debug] %s ActivateAbility() : called"), *myName); // debug

    TObjectPtr<UAbilitySystemComponent> asc = GetAbilitySystemComponentFromActorInfo();
    asc->AddLooseGameplayTag(tag_stateAttacking);

    hitchecker = USLAT_MeeleSweep_hit_checker::Create(
        this,
        socket_weapon_base, socket_weapon_tip, socket_weapon_length,
        BoxHalfExtents);

    // 1. Wait for the 'Start' event from your ANS
    UAbilityTask_WaitGameplayEvent* WaitTraceStart = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, tag_traceStart);
    if (WaitTraceStart) {
        WaitTraceStart->EventReceived.AddDynamic(this, &USLGA_MeleeSweep::TraceStart);
        WaitTraceStart->ReadyForActivation();
    }
    else {
        UE_LOG(LogTemp, Display, TEXT("[SL debug] %s ActivateAbility() : trace start event listener failed"), *myName);
    }

    // 2. Wait for the 'End' event to stop everything
    UAbilityTask_WaitGameplayEvent* WaitTraceEnd = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, tag_traceEnd);
    if (WaitTraceEnd) {
        WaitTraceEnd->EventReceived.AddDynamic(this, &USLGA_MeleeSweep::TraceEnd);
        WaitTraceEnd->ReadyForActivation();
    }
    else {
        UE_LOG(LogTemp, Display, TEXT("[SL debug] %s ActivateAbility() : tarce end event listener failed"), *myName);
    }

    // wait for free to move event
    UAbilityTask_WaitGameplayEvent* WaitFreeToMove = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, tag_freeToMove);
    if (WaitFreeToMove) {
        WaitFreeToMove->EventReceived.AddDynamic(this, &USLGA_MeleeSweep::FreeToMove);
        WaitFreeToMove->ReadyForActivation();
    }
    else {
        UE_LOG(LogTemp, Display, TEXT("[SL debug] %s ActivateAbility() : free to move event listener failed"), *myName);
    }

    // combo input wait
    UAbilityTask_WaitGameplayEvent* WaitInputAsCombo = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, tag_inputAsCombo);
    if (WaitInputAsCombo) {
        WaitInputAsCombo->EventReceived.AddDynamic(this, &USLGA_MeleeSweep::InputAsCombo);
        WaitInputAsCombo->ReadyForActivation();
    }
    else {
        UE_LOG(LogTemp, Display, TEXT("[SL debug] %s ActivateAbility() : combo input start event listener failed"), *myName);
    }

    // combo timing wait
    UAbilityTask_WaitGameplayEvent* WaitTryActivateCombo = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, tag_tryActivateCombo);
    if (WaitTryActivateCombo) {
        WaitTryActivateCombo->EventReceived.AddDynamic(this, &USLGA_MeleeSweep::TryActivateCombo);
        WaitTryActivateCombo->ReadyForActivation();
    }
    else {
        UE_LOG(LogTemp, Display, TEXT("[SL debug] %s ActivateAbility() : combo input start event listener failed"), *myName);
    }
}

void USLGA_MeleeSweep::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    TObjectPtr<UAbilitySystemComponent> asc = GetAbilitySystemComponentFromActorInfo();
    if (asc->HasMatchingGameplayTag(tag_stateAttacking)) asc->RemoveLooseGameplayTag(tag_stateAttacking);
    if (asc->HasMatchingGameplayTag(tag_inputAsCombo)) asc->RemoveLooseGameplayTag(tag_inputAsCombo); 
    if (asc->HasMatchingGameplayTag(tag_comboGrant)) asc->RemoveLooseGameplayTag(tag_comboGrant);
    if (asc->HasMatchingGameplayTag(tag_freeToMove)) asc->RemoveLooseGameplayTag(tag_freeToMove);

    hitchecker->EndTask();
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

void USLGA_MeleeSweep::FreeToMove(FGameplayEventData Payload)
{
    TObjectPtr<UAbilitySystemComponent> asc = GetAbilitySystemComponentFromActorInfo();

    asc->AddLooseGameplayTag(tag_freeToMove);

    if (asc->HasMatchingGameplayTag(tag_tryingToMove) &&
        !asc->HasMatchingGameplayTag(tag_comboGrant)) 
    {
        TObjectPtr<const UAnimMontage> targetMontage = Cast<UAnimMontage>(Payload.OptionalObject);

        if (targetMontage == asc->GetCurrentMontage()) {
            asc->CurrentMontageStop(0.2f);
        }
    }
}

void USLGA_MeleeSweep::InputAsCombo(FGameplayEventData Payload)
{
    TObjectPtr<UAbilitySystemComponent> asc = GetAbilitySystemComponentFromActorInfo();
    
    asc->AddLooseGameplayTag(tag_inputAsCombo);
    UE_LOG(LogTemp, Display, TEXT("[SL debug] nofity response with add loose tag = %s"), *tag_inputAsCombo.ToString());
}

void USLGA_MeleeSweep::TryActivateCombo(FGameplayEventData Payload)
{   
    TObjectPtr<UAbilitySystemComponent> asc = GetAbilitySystemComponentFromActorInfo();

    asc->RemoveLooseGameplayTag(tag_inputAsCombo);
    UE_LOG(LogTemp, Display, TEXT("[SL debug] TryActivateCombo() : remove loose tag = %s"), *tag_inputAsCombo.ToString());

    if (asc->HasMatchingGameplayTag(tag_comboGrant)) {

        asc->RemoveLooseGameplayTag(tag_comboGrant);
        UE_LOG(LogTemp, Display, TEXT("[SL debug] TryActivateCombo() : activating combo = %s"), *tag_comboAbility.ToString());
        asc->TryActivateAbilitiesByTag(FGameplayTagContainer(tag_comboAbility));
    }
}

void USLAT_MeeleSweep_hit_checker::EffectOnHit(AActor* hitActor)
{
    FString myName = this->GetName();

    FString actorName = hitActor->GetName();
    UE_LOG(LogTemp, Display, TEXT("[SL debug] %s EffectOnHit() : hit = %s"), *myName, *actorName);
}