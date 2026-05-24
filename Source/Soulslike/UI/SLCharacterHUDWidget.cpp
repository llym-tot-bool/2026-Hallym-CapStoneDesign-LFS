// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/SLCharacterHUDWidget.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Boss.h"
#include "SLCharacterAttributeSet.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

void USLCharacterHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	RefreshStatusBars();
	RefreshBossStatus();
}

void USLCharacterHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	RefreshStatusBars();
	RefreshBossStatus();
}

void USLCharacterHUDWidget::RefreshStatusBars()
{
	if (!ResolveAttributeSources() || !CachedAttributeSet)
	{
		UpdateStatusBars(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
		return;
	}

	const float Health = CachedAttributeSet->GetHealth();
	const float MaxHealth = CachedAttributeSet->GetMaxHealth();
	const float Stamina = CachedAttributeSet->GetStamina();
	const float MaxStamina = CachedAttributeSet->GetMaxStamina();
	const float Mana = CachedAttributeSet->GetMana();
	const float MaxMana = CachedAttributeSet->GetMaxMana();

	UpdateStatusBars(
		SafePercent(Health, MaxHealth),
		SafePercent(Stamina, MaxStamina),
		SafePercent(Mana, MaxMana),
		Health,
		MaxHealth,
		Stamina,
		MaxStamina,
		Mana,
		MaxMana);
}

bool USLCharacterHUDWidget::ResolveAttributeSources()
{
	if (CachedAbilitySystem.IsValid() && CachedAttributeSet)
	{
		return true;
	}

	APlayerController* OwnerPC = GetOwningPlayer();
	if (!OwnerPC)
	{
		return false;
	}

	APlayerState* OwnerPS = OwnerPC->GetPlayerState<APlayerState>();
	if (!OwnerPS)
	{
		return false;
	}

	IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(OwnerPS);
	if (!AbilityInterface)
	{
		return false;
	}

	UAbilitySystemComponent* AbilitySystem = AbilityInterface->GetAbilitySystemComponent();
	if (!AbilitySystem)
	{
		return false;
	}

	CachedAbilitySystem = AbilitySystem;
	CachedAttributeSet = AbilitySystem->GetSet<USLCharacterAttributeSet>();
	return CachedAttributeSet != nullptr;
}

float USLCharacterHUDWidget::SafePercent(float Current, float Max) const
{
	if (Max <= 0.0f)
	{
		return 0.0f;
	}

	return FMath::Clamp(Current / Max, 0.0f, 1.0f);
}

void USLCharacterHUDWidget::RefreshBossStatus()
{
	ABoss* Boss = ResolveTrackedBoss();
	if (!IsValid(Boss))
	{
		UpdateBossStatus(false, FText::GetEmpty(), 0.0f, 0.0f, 1.0f);
		return;
	}

	UAbilitySystemComponent* BossASC = Boss->GetAbilitySystemComponent();
	const USLCharacterAttributeSet* BossAttributeSet = BossASC ? BossASC->GetSet<USLCharacterAttributeSet>() : nullptr;
	if (!BossAttributeSet)
	{
		UpdateBossStatus(false, FText::GetEmpty(), 0.0f, 0.0f, 1.0f);
		return;
	}

	const float BossHealth = BossAttributeSet->GetHealth();
	const float BossMaxHealth = BossAttributeSet->GetMaxHealth();
	const bool bAlive = BossHealth > 0.0f;
	if (!bAlive || !IsBossInHudRange(Boss))
	{
		UpdateBossStatus(false, FText::GetEmpty(), 0.0f, 0.0f, 1.0f);
		return;
	}

	UpdateBossStatus(
		true,
		FText::Format(NSLOCTEXT("SLCharacterHUDWidget", "BossDisplayNameWithBrackets", "<{0}>"), Boss->GetBossDisplayName()),
		SafePercent(BossHealth, BossMaxHealth),
		BossHealth,
		BossMaxHealth);
}

ABoss* USLCharacterHUDWidget::ResolveTrackedBoss()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	const APawn* OwnerPawn = GetOwningPlayerPawn();
	if (!OwnerPawn)
	{
		return CachedBoss.IsValid() ? CachedBoss.Get() : nullptr;
	}

	TArray<AActor*> BossActors;
	UGameplayStatics::GetAllActorsOfClass(World, ABoss::StaticClass(), BossActors);

	ABoss* BestBoss = nullptr;
	float BestDistSq = TNumericLimits<float>::Max();

	for (AActor* Actor : BossActors)
	{
		ABoss* Boss = Cast<ABoss>(Actor);
		if (!IsValid(Boss))
		{
			continue;
		}

		UAbilitySystemComponent* BossASC = Boss->GetAbilitySystemComponent();
		const USLCharacterAttributeSet* BossAttributeSet = BossASC ? BossASC->GetSet<USLCharacterAttributeSet>() : nullptr;
		if (!BossAttributeSet || BossAttributeSet->GetHealth() <= 0.0f)
		{
			continue;
		}

		const float DistSq = FVector::DistSquared2D(OwnerPawn->GetActorLocation(), Boss->GetActorLocation());
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			BestBoss = Boss;
		}
	}

	CachedBoss = BestBoss;
	return BestBoss;
}

bool USLCharacterHUDWidget::IsBossInHudRange(const ABoss* Boss) const
{
	if (!IsValid(Boss))
	{
		return false;
	}

	const APawn* OwnerPawn = GetOwningPlayerPawn();
	if (!OwnerPawn)
	{
		return false;
	}

	if (BossHudVisibleDistance <= 0.0f)
	{
		return true;
	}

	const float DistSq2D = FVector::DistSquared2D(OwnerPawn->GetActorLocation(), Boss->GetActorLocation());
	return DistSq2D <= FMath::Square(BossHudVisibleDistance);
}
