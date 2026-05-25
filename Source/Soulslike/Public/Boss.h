// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Boss.generated.h"

class UAbilitySystemComponent;
class USLCharacterAttributeSet;
class UAnimMontage;
struct FGameplayTag;
struct FTimerHandle;
struct FOnAttributeChangeData;

UCLASS()
class SOULSLIKE_API ABoss : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ABoss();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintPure, Category="Boss|UI")
	FText GetBossDisplayName() const { return BossDisplayName; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<USLCharacterAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AI")
	bool bEnablePlayerChase = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AI", meta = (ClampMin = 100.0))
	float MaxChaseDistance = 2500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AI", meta = (ClampMin = 10.0))
	float ChaseAcceptanceRadius = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AI|Movement", meta = (ClampMin = 0.0))
	float DefaultMoveSpeed = 270.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AI|Movement", meta = (ClampMin = 0.0))
	float ChaseMoveSpeed = 420.0f;

	// If true, boss rotates to face its movement direction while moving.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AI|Movement")
	bool bFaceMovementDirection = true;

	// Rotation interpolation speed while facing movement direction.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AI|Movement", meta = (ClampMin = 0.0))
	float MoveFacingInterpSpeed = 10.0f;

	// Half-angle of forward detection cone in degrees.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AI|Perception", meta = (ClampMin = 1.0, ClampMax = 180.0))
	float ChaseDetectionHalfAngleDeg = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AI|Perception|Debug")
	bool bDebugDrawDetectionRange = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AI|Combat|Debug")
	bool bDebugDrawAttackRange = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AI|Combat", meta = (ClampMin = 0.0))
	float BasicAttackRange = 130.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AI|Combat", meta = (ClampMin = 0.1))
	float BasicAttackCooldown = 1.6f;

	// Extra delay after attack montage ends before next attack can start.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AI|Combat", meta = (ClampMin = 0.0))
	float BasicAttackPostMontageDelay = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AI|Combat", meta = (ClampMin = 0.0))
	float BasicAttackBaseDamage = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AI|Combat", meta = (ClampMin = 0.0))
	float BasicAttackPowerScale = 0.9f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AI|Combat|Animation", meta = (ClampMin = 0.0))
	float BasicAttackHitDelay = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AI|Combat", meta = (ClampMin = 0.0))
	float MoveBehindTargetDistance = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AI", meta = (ClampMin = 0.1))
	float TargetSearchInterval = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AI|Wander")
	bool bEnablePeriodicMove = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AI|Wander", meta = (ClampMin = 0.1))
	float PeriodicMoveInterval = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AI|Wander", meta = (ClampMin = 100.0))
	float PeriodicMoveRadius = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Animation")
	TObjectPtr<UAnimMontage> IntroMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|UI")
	FText BossDisplayName = FText::FromString(TEXT("Ghost of Ash"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AI|Combat|Animation")
	TObjectPtr<UAnimMontage> BasicAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AI|Combat|Animation")
	TObjectPtr<UAnimMontage> BasicAttackMontageAlt;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AI|Combat|Animation")
	TObjectPtr<UAnimMontage> BasicAttackMontageAlt2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Animation")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AI|Combat|Animation")
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AI|Combat|Animation")
	TObjectPtr<UAnimMontage> GroggyMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Combat|Groggy", meta = (ClampMin = 0.0))
	float GroggyKnockbackDistance = 220.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Combat|Groggy", meta = (ClampMin = 0.05))
	float GroggyKnockbackDuration = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Combat|Stats", meta = (ClampMin = 1.0))
	float InitialHealthStat = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Combat|Stats", meta = (ClampMin = 1.0))
	float InitialGroggyStat = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|State", meta = (ClampMin = 0.1))
	float DeathDespawnDelay = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|AI|Combat|Animation")
	TObjectPtr<UAnimMontage> HitReactMontage;

	FTimerHandle ChaseTimer;
	FTimerHandle PeriodicMoveTimer;
	FTimerHandle BasicAttackHitTimer;
	FTimerHandle GroggyRecoverTimer;
	FTimerHandle DeathDespawnTimer;
	FTimerHandle DeathPoseFreezeTimer;
	float LastBasicAttackTime = -1000.0f;
	bool bBasicAttackInProgress = false;
	int32 BasicAttackDamageNotifyCount = 0;
	bool bIsGroggy = false;
	bool bDeathMontagePlayed = false;
	TWeakObjectPtr<AActor> PendingAttackTarget;

	UFUNCTION()
	void UpdateChaseTarget();

	UFUNCTION()
	void MoveToRandomReachableLocation();

	float ComputeBasicAttackDamage() const;
	bool IsTargetTouchingAttackRange(AActor* TargetActor) const;
	void ResolveBasicAttackHit();
	void OnBasicAttackNotifyTimeout();
	void OnGroggyTagChanged(const FGameplayTag Tag, int32 NewCount);
	void OnHealthAttributeChanged(const FOnAttributeChangeData& ChangeData);
	void RecoverGroggyToMax();
	void HandleDeathState();
	void OnDeathDespawnTimerElapsed();
	void FreezeDeathPose();

public:
	UFUNCTION(BlueprintCallable, Category = "Boss|AI")
	float GetChaseAcceptanceRadius() const { return ChaseAcceptanceRadius; }

	UFUNCTION(BlueprintCallable, Category = "Boss|AI")
	bool IsPlayerChaseEnabled() const { return bEnablePlayerChase; }

	UFUNCTION(BlueprintCallable, Category = "Boss|AI|Movement")
	float GetDefaultMoveSpeed() const { return DefaultMoveSpeed; }

	UFUNCTION(BlueprintCallable, Category = "Boss|AI|Movement")
	float GetChaseMoveSpeed() const { return ChaseMoveSpeed; }

	UFUNCTION(BlueprintCallable, Category = "Boss|AI")
	float GetMaxChaseDistance() const { return MaxChaseDistance; }

	UFUNCTION(BlueprintCallable, Category = "Boss|AI|Perception")
	float GetChaseDetectionHalfAngleDeg() const { return ChaseDetectionHalfAngleDeg; }

	UFUNCTION(BlueprintCallable, Category = "Boss|AI|Perception|Debug")
	bool IsDebugDrawDetectionRangeEnabled() const { return bDebugDrawDetectionRange; }

	UFUNCTION(BlueprintCallable, Category = "Boss|AI|Combat|Debug")
	bool IsDebugDrawAttackRangeEnabled() const { return bDebugDrawAttackRange; }

	UFUNCTION(BlueprintCallable, Category = "Boss|AI|Combat")
	float GetBasicAttackRange() const { return BasicAttackRange; }

	UFUNCTION(BlueprintCallable, Category = "Boss|AI|Combat")
	bool TryBasicAttack(AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "Boss|AI|Combat")
	bool IsTargetInBasicAttackContact(AActor* TargetActor) const;

	UFUNCTION(BlueprintCallable, Category = "Boss|AI|Combat")
	bool IsBasicAttackInProgress() const { return bBasicAttackInProgress; }

	UFUNCTION(BlueprintCallable, Category = "Boss|AI|Combat|Animation")
	bool IsBasicAttackMontagePlaying() const;

	UFUNCTION(BlueprintCallable, Category = "Boss|AI|Combat")
	bool IsGroggy() const { return bIsGroggy; }

	/** Called by boss attack montage AnimNotify at the exact hit frame. */
	UFUNCTION(BlueprintCallable, Category = "Boss|AI|Combat")
	void OnBasicAttackDamageNotify();

	/** Called by boss montage AnimNotify to move behind current attack target. */
	UFUNCTION(BlueprintCallable, Category = "Boss|AI|Combat")
	void OnMoveBehindTargetNotify();

	/** Called by boss attack montage AnimNotify to reset play rate to default. */
	UFUNCTION(BlueprintCallable, Category = "Boss|AI|Combat|Animation")
	void OnBasicAttackSpeedResetNotify();

	UFUNCTION(BlueprintCallable, Category = "Boss|AI")
	void StartChase();

	UFUNCTION(BlueprintCallable, Category = "Boss|AI")
	void StopChase();

	UFUNCTION(BlueprintCallable, Category = "Boss|AI|Wander")
	void StartPeriodicMove();

	UFUNCTION(BlueprintCallable, Category = "Boss|AI|Wander")
	void StopPeriodicMove();

	UFUNCTION(BlueprintCallable, Category = "Boss|Animation")
	bool PlayBossMontage(UAnimMontage* MontageToPlay, float PlayRate = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Boss|Animation")
	bool PlayDefaultAttackMontage(float PlayRate = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Boss|Animation")
	bool PlayDeathMontage(float PlayRate = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Boss|Animation")
	bool PlayGroggyMontage(float PlayRate = 1.0f);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayBasicAttackMontage(UAnimMontage* MontageToPlay, float PlayRate = 1.0f);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayGroggyMontage(UAnimMontage* MontageToPlay, float PlayRate = 1.0f);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayHitReactMontage(UAnimMontage* MontageToPlay, float PlayRate = 1.0f);

	UFUNCTION(BlueprintPure, Category = "Boss|Animation")
	float GetGroundSpeed() const;

	UFUNCTION(BlueprintPure, Category = "Boss|Animation")
	bool IsMoving() const;

	UFUNCTION(BlueprintPure, Category = "Boss|State")
	bool IsDead() const;
};
