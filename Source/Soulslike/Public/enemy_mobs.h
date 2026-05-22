// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "enemy_mobs.generated.h"

class UAbilitySystemComponent;
class USLCharacterAttributeSet;
class UAnimMontage;
class UWidgetComponent;
class USLEnemyHPBarWidget;
class UUserWidget;
class UProgressBar;
struct FGameplayTag;
struct FTimerHandle;
struct FOnAttributeChangeData;

UCLASS()
class SOULSLIKE_API Aenemy_mobs : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	Aenemy_mobs();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<USLCharacterAttributeSet> AttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> HealthBarWidgetComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float ChaseAcceptanceRadius = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	bool bEnablePlayerChase = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Movement", meta = (ClampMin = 0.0))
	float DefaultMoveSpeed = 130.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Movement", meta = (ClampMin = 0.0))
	float ChaseMoveSpeed = 260.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI", meta = (ClampMin = 100.0))
	float MaxChaseDistance = 1200.0f;

	// Half-angle of forward detection cone in degrees.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Perception", meta = (ClampMin = 1.0, ClampMax = 180.0))
	float ChaseDetectionHalfAngleDeg = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Perception|Debug")
	bool bDebugDrawDetectionRange = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Combat|Debug")
	bool bDebugDrawAttackRange = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Combat", meta = (ClampMin = 0.0))
	float BasicAttackRange = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Combat", meta = (ClampMin = 0.1))
	float BasicAttackCooldown = 1.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Combat", meta = (ClampMin = 0.0))
	float BasicAttackBaseDamage = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Combat", meta = (ClampMin = 0.0))
	float BasicAttackPowerScale = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Combat|Stats", meta = (ClampMin = 0.0))
	float InitialPowerStat = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Combat|Stats", meta = (ClampMin = 1.0))
	float InitialGroggyStat = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Combat|Animation")
	TObjectPtr<UAnimMontage> BasicAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Combat|Animation")
	TObjectPtr<UAnimMontage> BasicAttackMontageAlt;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Combat|Animation")
	TObjectPtr<UAnimMontage> GroggyMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Combat|Animation")
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Combat|Animation")
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Combat|Animation", meta = (ClampMin = 0.0))
	float BasicAttackHitDelay = 0.25f;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Combat|Groggy", meta = (ClampMin = 0.0))
	float GroggyKnockbackDistance = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Combat|Groggy", meta = (ClampMin = 0.05))
	float GroggyKnockbackDuration = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Wander")
	bool bEnablePeriodicMove = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Wander", meta = (ClampMin = 0.1))
	float PeriodicMoveInterval = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Wander", meta = (ClampMin = 100.0))
	float PeriodicMoveRadius = 1000.0f;

	FTimerHandle PeriodicMoveTimer;

	UFUNCTION()
	void MoveToRandomReachableLocation();

public:
	UFUNCTION(BlueprintCallable, Category = "AI")
	float GetChaseAcceptanceRadius() const { return ChaseAcceptanceRadius; }

	UFUNCTION(BlueprintCallable, Category = "AI")
	bool IsPlayerChaseEnabled() const { return bEnablePlayerChase; }

	UFUNCTION(BlueprintCallable, Category = "AI|Movement")
	float GetDefaultMoveSpeed() const { return DefaultMoveSpeed; }

	UFUNCTION(BlueprintCallable, Category = "AI|Movement")
	float GetChaseMoveSpeed() const { return ChaseMoveSpeed; }

	UFUNCTION(BlueprintCallable, Category = "AI")
	float GetMaxChaseDistance() const { return MaxChaseDistance; }

	UFUNCTION(BlueprintCallable, Category = "AI|Perception")
	float GetChaseDetectionHalfAngleDeg() const { return ChaseDetectionHalfAngleDeg; }

	UFUNCTION(BlueprintCallable, Category = "AI|Perception|Debug")
	bool IsDebugDrawDetectionRangeEnabled() const { return bDebugDrawDetectionRange; }

	UFUNCTION(BlueprintCallable, Category = "AI|Combat|Debug")
	bool IsDebugDrawAttackRangeEnabled() const { return bDebugDrawAttackRange; }

	UFUNCTION(BlueprintCallable, Category = "AI|Combat")
	float GetBasicAttackRange() const { return BasicAttackRange; }

	UFUNCTION(BlueprintCallable, Category = "AI|Combat")
	bool TryBasicAttack(AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "AI|Combat")
	bool IsTargetInBasicAttackContact(AActor* TargetActor) const;

	UFUNCTION(BlueprintCallable, Category = "AI|Combat")
	bool IsBasicAttackInProgress() const { return bBasicAttackInProgress; }

	UFUNCTION(BlueprintCallable, Category = "AI|Combat|Animation")
	bool IsBasicAttackMontagePlaying() const;

	UFUNCTION(BlueprintCallable, Category = "AI|Combat")
	bool IsGroggy() const { return bIsGroggy; }

	UFUNCTION(BlueprintPure, Category = "AI|State")
	bool IsDead() const;

	/** Called by attack montage AnimNotify at the exact hit frame. */
	UFUNCTION(BlueprintCallable, Category = "AI|Combat|Animation")
	void OnBasicAttackDamageNotify();

	/** Called by attack montage AnimNotify to reset play rate to default. */
	UFUNCTION(BlueprintCallable, Category = "AI|Combat|Animation")
	void OnBasicAttackSpeedResetNotify();

	UFUNCTION(BlueprintCallable, Category = "AI|Wander")
	void StartPeriodicMove();

	UFUNCTION(BlueprintCallable, Category = "AI|Wander")
	void StopPeriodicMove();

private:
	float LastBasicAttackTime = -1000.0f;
	bool bBasicAttackInProgress = false;
	int32 BasicAttackDamageNotifyCount = 0;
	bool bIsGroggy = false;
	bool bDeathMontagePlayed = false;
	FTimerHandle BasicAttackHitTimer;
	FTimerHandle GroggyRecoverTimer;
	TWeakObjectPtr<AActor> PendingAttackTarget;
	TObjectPtr<USLEnemyHPBarWidget> HealthBarWidgetInstance;
	TObjectPtr<UProgressBar> HealthBarProgressBar = nullptr;

	float ComputeBasicAttackDamage() const;
	void RefreshHealthBarUI() const;
	bool IsTargetTouchingAttackRange(AActor* TargetActor) const;
	void ResolveBasicAttackHit();
	void OnBasicAttackNotifyTimeout();
	void OnGroggyTagChanged(const FGameplayTag Tag, int32 NewCount);
	void OnHealthAttributeChanged(const FOnAttributeChangeData& ChangeData);
	void RecoverGroggyToMax();
	void HandleDeathState();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayBasicAttackMontage(UAnimMontage* MontageToPlay, float PlayRate = 1.0f);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayGroggyMontage(UAnimMontage* MontageToPlay, float PlayRate = 1.0f);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayHitReactMontage(UAnimMontage* MontageToPlay, float PlayRate = 1.0f);
};
