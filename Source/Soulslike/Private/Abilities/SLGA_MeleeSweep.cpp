


#include "Abilities/SLGA_MeleeSweep.h"
#include "SoulslikeCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Weapons/SLWeaponTypes.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Soulslike.h"

USLAT_Meele_hit_checker* USLAT_Meele_hit_checker::Create(UGameplayAbility* OwningAbility,
    FName socket_start_name, FName socket_end_name,
    float trace_length, FVector boxHalfExtents,
    USoundBase* InHitSound, UNiagaraSystem* vfx_onhit)
{
    USLAT_Meele_hit_checker* hit_checker = NewAbilityTask<USLAT_Meele_hit_checker>(OwningAbility);
    hit_checker->socket_base_name = socket_start_name;
    hit_checker->socket_tip_name = socket_end_name;
    hit_checker->trace_length = trace_length;
    hit_checker->boxHalfExtents = boxHalfExtents;
    hit_checker->isScanning = false;
    hit_checker->HitSound = InHitSound;
    hit_checker->VFX_onhit = vfx_onhit;

    hit_checker->actorsToIgnore.Empty();
    if (!hit_checker->IgnoreSelf()) return nullptr;

    hit_checker->bTickingTask = true;
    return hit_checker;
}

bool USLAT_Meele_hit_checker::IgnoreSelf()
{
    AActor* avatar = GetAvatarActor();
    if (!avatar) return false;
    actorsToIgnore.Add(avatar);
    return true;
}

void USLAT_Meele_hit_checker::SetIsScanning(const bool value)
{
    isScanning = value;
}

void USLAT_Meele_hit_checker::ChangeTraceSpec(FName new_base_name, FName new_tip_name, float new_trace_length, FVector new_boxHalfExtents)
{
    socket_base_name = new_base_name;
    socket_tip_name = new_tip_name;
    trace_length = new_trace_length;
    boxHalfExtents = new_boxHalfExtents;
}

void USLAT_Meele_hit_checker::ChangeHitSound(USoundBase* new_sound)
{
    HitSound = new_sound;
}

void USLAT_Meele_hit_checker::ChangeVFX(UNiagaraSystem* new_vfx)
{
    VFX_onhit = new_vfx;
}

void USLAT_Meele_hit_checker::FlushIgnoreList()
{
    actorsToIgnore.Empty();
    bool ret = IgnoreSelf();
    ensureOrQuit(ret);
}

void USLAT_Meele_hit_checker::TickTask(float DeltaTime)
{
    Super::TickTask(DeltaTime);

    if (!isScanning) return;

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
            IAbilitySystemInterface* Interface = Cast<IAbilitySystemInterface>(hitActor);
            if (!Interface) continue;
            UAbilitySystemComponent* hitASC = Interface->GetAbilitySystemComponent();
            if (!hitASC) continue;
            if (actorsToIgnore.Contains(hitActor)) continue;

            actorsToIgnore.Add(hitActor);
            EffectOnHit(hitActor, hitASC, hitresult.ImpactPoint);
        }
    }
}

void USLAT_Meele_hit_checker::EffectOnHit(AActor* hitActor, UAbilitySystemComponent* hitASC, const FVector& HitLocation)
{
    FString myName = this->GetName();

    FString actorName = hitActor->GetName();
    UE_LOG(LogTemp, Display, TEXT("[SL debug] %s EffectOnHit() : hit = %s"), *myName, *actorName);

    if (HitSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, HitSound, HitLocation);
    }

    if (VFX_onhit) {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            VFX_onhit,
            HitLocation,
            FRotator(0, 0, 0),
            FVector(1.0f), // Scale
            true,          // Auto Destroy
            true,          // Auto Activate
            ENCPoolMethod::None,
            true           // Pre-Cull Check
        );
    }
}

void USLGA_MeleeSweep::InterruptAsCombo()
{
    bInterruptAsCombo = true;
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void USLGA_MeleeSweep::InterruptAsCancel()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
}

void USLGA_MeleeSweep::setRootMotion()
{
    TObjectPtr<UAbilitySystemComponent> asc = GetAbilitySystemComponentFromActorInfo();
    if (!asc) return;

    if (!asc->HasMatchingGameplayTag(tag_RootMotion)) {
        asc->AddLooseGameplayTag(tag_RootMotion);
    }

    if (asc->HasMatchingGameplayTag(tag_IsMoving)) {
        asc->RemoveLooseGameplayTag(tag_IsMoving);
    }
}

void USLGA_MeleeSweep::removeRootMotion()
{
    TObjectPtr<UAbilitySystemComponent> asc = GetAbilitySystemComponentFromActorInfo();
    if (!asc) return;

    if (asc->HasMatchingGameplayTag(tag_RootMotion)) {
        asc->RemoveLooseGameplayTag(tag_RootMotion);
    }
}

bool USLGA_MeleeSweep::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
    const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
    if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)) return false;

    if (ActorInfo)
    {
        if (UAbilitySystemComponent* asc = ActorInfo->AbilitySystemComponent.Get())
        {
            const float CurrentStamina = asc->GetNumericAttribute(USLCharacterAttributeSet::GetStaminaAttribute());
            if (CurrentStamina < StaminaCost)
            {
                return false;
            }
        }
    }
    return true;
}

void USLGA_MeleeSweep::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    state = ESL_Melee_State::Anticipation;
    traceState = ESL_Melee_TraceState::none;
    bInterruptAsCombo = false;
    setRootMotion();

    // delegate binding
    ASC = GetAbilitySystemComponentFromActorInfo(); ensureOrQuit(ASC);
    SLPS = Cast<ASoulslikePlayerState>(ASC->GetOwner()); ensureOrQuit(SLPS)
    SLPS->delegate_MeleeSweep_State.AddUObject(this, &USLGA_MeleeSweep::ChangeState);
    SLPS->delegate_MeleeSweep_TraceState.AddUObject(this, &USLGA_MeleeSweep::ChangeTraceState);

    ASoulslikeCharacter* SLChar = Cast<ASoulslikeCharacter>(ActorInfo->AvatarActor); ensureOrQuit(SLChar);
    SLChar->delegate_CharacterMove.AddUObject(this, &USLGA_MeleeSweep::OnCharacteMove);

    hitchecker = USLAT_Meele_hit_checker::Create(
        this,
        socket_weapon_base, socket_weapon_tip, socket_weapon_length,
        BoxHalfExtents, HitSound, VFX_onhit);
    hitchecker->ReadyForActivation();
    
    FGameplayEffectContextHandle Ctx = ASC->MakeEffectContext();
    Ctx.AddSourceObject(ActorInfo->AvatarActor.Get());
    FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(StaminaCostGEClass, /*Level*/ GetAbilityLevel(), Ctx);
    if (SpecHandle.IsValid())
    {
        const FGameplayTag CostTag = FGameplayTag::RequestGameplayTag(SLCombatTags::SetByCaller_StaminaCost, /*ErrorIfNotFound*/ false);
        if (CostTag.IsValid())
        {
            SpecHandle.Data->SetSetByCallerMagnitude(CostTag, -StaminaCost);
        }
        ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    }
}

void USLGA_MeleeSweep::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    if (hitchecker) {
        hitchecker->EndTask();
    }

    // remove delegate biindings
    if (SLPS) {
        SLPS->delegate_MeleeSweep_State.RemoveAll(this);
        SLPS->delegate_MeleeSweep_TraceState.RemoveAll(this);
    }

    ASoulslikeCharacter* SLChar = Cast<ASoulslikeCharacter>(ActorInfo->AvatarActor);
    if (SLChar) {
        SLChar->delegate_CharacterMove.RemoveAll(this);
    }


    if (bInterruptAsCombo) {
        removeRootMotion();
    }
    else {
        switch (state) {
        case ESL_Melee_State::Anticipation:
            SLDEBUG("non combo End ability with sweep state : Anticipation")
            Recovery();
            break;
        case ESL_Melee_State::ComboInput:
            SLDEBUG("non combo End ability with sweep state : ComboInput")
            Recovery();
            break;
        case ESL_Melee_State::Translation:
            SLDEBUG("non combo End ability with sweep state : Translation")
            Recovery();
            break;
        case ESL_Melee_State::Recovery:
            break;
        }
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USLGA_MeleeSweep::ChangeState(ESL_Melee_State newstate)
{
    switch (newstate)
    {
    case ESL_Melee_State::ComboInput:
        UE_LOG(LogTemp, Display, TEXT("[SL debug] delegate arrived : ComboInput "));
        ComboInput();
        break;
    case ESL_Melee_State::Translation:
        UE_LOG(LogTemp, Display, TEXT("[SL debug] delegate arrived : Translation "));
        Translation();
        break;
    case ESL_Melee_State::Recovery:
        UE_LOG(LogTemp, Display, TEXT("[SL debug] delegate arrived : Recovery "));
        Recovery();
        break;
    default:
        UE_LOG(LogTemp, Display, TEXT("[SL debug] !!! melee sweep wrong tag"));
        break;
    }
}

void USLGA_MeleeSweep::ChangeTraceState(ESL_Melee_TraceState newState)
{
    UE_LOG(LogTemp, Display, TEXT("[SL debug] delegate for TRACE state arrived"));

    ensure(hitchecker);

    traceState = newState;
    switch (traceState)
    {
    case ESL_Melee_TraceState::none:
        UE_LOG(LogTemp, Display, TEXT("[SL debug] delegate response : trace end"));
        hitchecker->SetIsScanning(false);
        break;
    case ESL_Melee_TraceState::trace:
        UE_LOG(LogTemp, Display, TEXT("[SL debug] delegate response : trace start"));
        hitchecker->SetIsScanning(true);
        if (SwingSound) {
            UGameplayStatics::PlaySoundAtLocation(this, SwingSound, ASC->GetAvatarActor()->K2_GetActorLocation());
        }
        break;
    default:
        break;
    }
}

void USLGA_MeleeSweep::OnCharacteMove()
{
    UE_LOG(LogTemp, Display, TEXT("[SL debug] melee canceld with character move"));
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
}

void USLGA_MeleeSweep::ComboInput()
{
    state = ESL_Melee_State::ComboInput;
    delegate_ComboInput.Broadcast();
}

void USLGA_MeleeSweep::Translation()
{
    state = ESL_Melee_State::Translation;
    delegate_Translation.Broadcast();
}

void USLGA_MeleeSweep::Recovery()
{
    state = ESL_Melee_State::Recovery;
    SLDEBUG("Recovery boradcasted from = %s", *this->GetName());
    removeRootMotion();
    delegate_Recovery.Broadcast();
}


