


#include "Abilities/SLGA_MeleeMultiMontage.h"
#include "SoulslikeCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Weapons/SLWeaponTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Soulslike.h"



void USLGA_MeleeMultiMontage::InterruptAsCancel()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
}

void USLGA_MeleeMultiMontage::setRootMotion()
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

void USLGA_MeleeMultiMontage::removeRootMotion()
{
    TObjectPtr<UAbilitySystemComponent> asc = GetAbilitySystemComponentFromActorInfo();
    if (!asc) return;

    if (asc->HasMatchingGameplayTag(tag_RootMotion)) {
        asc->RemoveLooseGameplayTag(tag_RootMotion);
    }
}

bool USLGA_MeleeMultiMontage::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, 
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

void USLGA_MeleeMultiMontage::ActivateAbility(const FGameplayAbilitySpecHandle Handle, 
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, 
    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    state = ESL_Melee_State::Anticipation;
    traceState = ESL_Melee_TraceState::none;
    setRootMotion();

    lastMontageIdx = MontageAction_list.Num() - 1; ensureOrQuit(lastMontageIdx >= 0);
    currentMontageIdx = 0;

    for (FSL_MontageAction eachMontageAction : MontageAction_list) {
        ensureOrQuit(eachMontageAction.montage);
    }

    // delegate binding
    ASC = GetAbilitySystemComponentFromActorInfo(); ensureOrQuit(ASC);
    SLPS = Cast<ASoulslikePlayerState>(ASC->GetOwner()); ensureOrQuit(SLPS)
        SLPS->delegate_MeleeSweep_State.AddUObject(this, &USLGA_MeleeMultiMontage::ChangeState);
    SLPS->delegate_MeleeSweep_TraceState.AddUObject(this, &USLGA_MeleeMultiMontage::ChangeTraceState);

    ASoulslikeCharacter* SLChar = Cast<ASoulslikeCharacter>(ActorInfo->AvatarActor); ensureOrQuit(SLChar);
    SLChar->delegate_CharacterMove.AddUObject(this, &USLGA_MeleeMultiMontage::OnCharacteMove);

    AnimInst = SLChar->GetMesh()->GetAnimInstance(); ensureOrQuit(AnimInst);

    hitchecker = USLAT_Meele_hit_checker::Create(
        this,
        FName("None"), FName("None"), 0.0f,
        FVector(0, 0, 0), nullptr, nullptr);
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

    PlayFirstMontage();
}

void USLGA_MeleeMultiMontage::EndAbility(const FGameplayAbilitySpecHandle Handle, 
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, 
    bool bReplicateEndAbility, bool bWasCancelled)
{
    ensureOrQuit(hitchecker);
    hitchecker->EndTask();
    hitchecker = nullptr;

    ensureOrQuit(SLPS);
    // remove delegate biindings
    SLPS->delegate_MeleeSweep_State.RemoveAll(this);
    SLPS->delegate_MeleeSweep_TraceState.RemoveAll(this);

    ASoulslikeCharacter* SLChar = Cast<ASoulslikeCharacter>(ActorInfo->AvatarActor); ensureOrQuit(SLChar);
    SLChar->delegate_CharacterMove.RemoveAll(this);

    switch (state) {
    case ESL_Melee_State::Anticipation:
        SLDEBUG("End ability with Melee state : Anticipation");
        Recovery();
        break;
    case ESL_Melee_State::Recovery:
        break;
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USLGA_MeleeMultiMontage::PlayFirstMontage()
{

    SLDEBUG("play first montage")
    // Inside your Gameplay Ability's ActivateAbility function
    currentMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        this,
        NAME_None,      // Task Instance Name
        MontageAction_list[currentMontageIdx].montage,      // The Montage asset
        1.f,            // Rate
        NAME_None,      // Start Section
        true            // bStopWhenAbilityEnds
    );

    // Bind to delegates (OnCompleted, OnInterrupted, etc.)
    ObserveMontage();
    hitchecker->ChangeTraceSpec(
        MontageAction_list[currentMontageIdx].socket_base_name,
        MontageAction_list[currentMontageIdx].socket_tip_name,
        MontageAction_list[currentMontageIdx].trace_length,
        MontageAction_list[currentMontageIdx].boxHalfExtents
    );
    hitchecker->FlushIgnoreList();
    hitchecker->ChangeHitSound(MontageAction_list[currentMontageIdx].HitSound);
    hitchecker->ChangeVFX(MontageAction_list[currentMontageIdx].VFX_onhit);
    currentMontageTask->ReadyForActivation();
}

void USLGA_MeleeMultiMontage::PlayNextMontage()
{
    ensureOrQuit(currentMontageTask);

    ObserveQuit();
    currentMontageTask->EndTask();
    currentMontageTask = nullptr;

    ensureOrQuit(currentMontageIdx < lastMontageIdx);

    currentMontageIdx++;
    SLDEBUG("play next montage. idx = %d", currentMontageIdx);
    currentMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        this,
        NAME_None,      // Task Instance Name
        MontageAction_list[currentMontageIdx].montage,      // The Montage asset
        1.f,            // Rate
        NAME_None,      // Start Section
        true            // bStopWhenAbilityEnds
    );

    ObserveMontage();
    currentMontageTask->ReadyForActivation();
    hitchecker->ChangeTraceSpec(
        MontageAction_list[currentMontageIdx].socket_base_name,
        MontageAction_list[currentMontageIdx].socket_tip_name,
        MontageAction_list[currentMontageIdx].trace_length,
        MontageAction_list[currentMontageIdx].boxHalfExtents
    );
    hitchecker->FlushIgnoreList();
    hitchecker->ChangeHitSound(MontageAction_list[currentMontageIdx].HitSound);
}

void USLGA_MeleeMultiMontage::ObserveMontage()
{
    ensureOrQuit(currentMontageTask);
    ensureOrQuit(SLPS);

    currentMontageTask->OnCompleted.AddDynamic(this, &USLGA_MeleeMultiMontage::OnMontageFinished);
    currentMontageTask->OnInterrupted.AddDynamic(this, &USLGA_MeleeMultiMontage::OnMontageFinished);
    SLPS->delegate_MeleeSweep_State.AddUObject(this, &USLGA_MeleeMultiMontage::ChangeState);
}

void USLGA_MeleeMultiMontage::ObserveQuit()
{
    ensureOrQuit(currentMontageTask);
    ensureOrQuit(SLPS);

    currentMontageTask->OnCompleted.RemoveAll(this);
    currentMontageTask->OnInterrupted.RemoveAll(this);
    SLPS->delegate_MeleeSweep_State.RemoveAll(this);
}

void USLGA_MeleeMultiMontage::ChangeState(ESL_Melee_State newstate)
{
    switch (newstate)
    {
    case ESL_Melee_State::CutMontage:
        SLDEBUG("delegate arrived : CutMontage");
        PlayNextMontage();
        break;
    case ESL_Melee_State::Recovery:
        UE_LOG(LogTemp, Display, TEXT("[SL debug] delegate arrived : Recovery "));
        state = newstate;
        Recovery();
        break;
    default:
        UE_LOG(LogTemp, Display, TEXT("[SL debug] !!! melee sweep wrong tag"));
        break;
    }
}

void USLGA_MeleeMultiMontage::ChangeTraceState(ESL_Melee_TraceState newState)
{
    UE_LOG(LogTemp, Display, TEXT("[SL debug] delegate for TRACE state arrived"));

    ensureOrQuit(hitchecker);

    traceState = newState;
    switch (traceState)
    {
    case ESL_Melee_TraceState::none:
        UE_LOG(LogTemp, Display, TEXT("[SL debug] delegate response : trace end"));
        hitchecker->SetIsScanning(false);
        break;
    case ESL_Melee_TraceState::trace:
        UE_LOG(LogTemp, Display, TEXT("[SL debug] delegate response : trace start"));
        UGameplayStatics::PlaySoundAtLocation(this, SwingSound, ASC->GetAvatarActor()->K2_GetActorLocation());
        hitchecker->SetIsScanning(true);
        break;
    default:
        break;
    }
}

void USLGA_MeleeMultiMontage::OnCharacteMove()
{
    UE_LOG(LogTemp, Display, TEXT("[SL debug] melee canceld with character move"));
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
}

void USLGA_MeleeMultiMontage::OnMontageFinished()
{
    ensureOrQuit(hitchecker);

    hitchecker->SetIsScanning(false);

    //Recovery();
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void USLGA_MeleeMultiMontage::Recovery()
{
	SLDEBUG("Recovery boradcasted from = %s", *this->GetName());
	removeRootMotion();
	delegate_Recovery.Broadcast();
}


