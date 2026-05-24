// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/SLEnemyHPBarWidget.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Widgets/Layout/SBox.h"

TSharedRef<SWidget> USLEnemyHPBarWidget::RebuildWidget()
{
	SlateHealthBar =
		SNew(SProgressBar)
		.Percent(CachedHealthPercent)
		.FillColorAndOpacity(FLinearColor(8.0f, 0.0f, 0.0f, 1.0f));

	return SNew(SBox)
		.WidthOverride(120.0f)
		.HeightOverride(12.0f)
		[
			SlateHealthBar.ToSharedRef()
		];
}

void USLEnemyHPBarWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	SlateHealthBar.Reset();
}

void USLEnemyHPBarWidget::SetHealthPercent(float InPercent)
{
	CachedHealthPercent = FMath::Clamp(InPercent, 0.0f, 1.0f);

	if (SlateHealthBar.IsValid())
	{
		SlateHealthBar->SetFillColorAndOpacity(FLinearColor(8.0f, 0.0f, 0.0f, 1.0f));
		SlateHealthBar->SetPercent(CachedHealthPercent);
	}
}
