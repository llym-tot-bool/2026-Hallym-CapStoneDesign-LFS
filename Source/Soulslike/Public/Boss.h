// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Boss.generated.h"

class UAbilitySystemComponent;
class USLCharacterAttributeSet;
class UAnimMontage;
struct FTimerHandle;

UCLASS()
class SOULSLIKE_API ABoss : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ABoss();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Animation")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Animation")
	TObjectPtr<UAnimMontage> DeathMontage;

	FTimerHandle ChaseTimer;
	FTimerHandle PeriodicMoveTimer;

	UFUNCTION()
	void UpdateChaseTarget();

	UFUNCTION()
	void MoveToRandomReachableLocation();

public:
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

	UFUNCTION(BlueprintPure, Category = "Boss|Animation")
	float GetGroundSpeed() const;

	UFUNCTION(BlueprintPure, Category = "Boss|Animation")
	bool IsMoving() const;

	UFUNCTION(BlueprintPure, Category = "Boss|State")
	bool IsDead() const;
};
