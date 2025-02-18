// Copyright Alien Shores


#include "UI/Library/AuraWidgetFunctionLibrary.h"

#include "Components/OverlaySlot.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "Brushes/SlateColorBrush.h"

void UAuraWidgetFunctionLibrary::SetBoxSizeOverride(USizeBox* SizeBox, float Height, float Width)
{
	SizeBox->SetWidthOverride(Width);
	SizeBox->SetHeightOverride(Height);
}

void UAuraWidgetFunctionLibrary::SetWidgetPadding(UWidget* Widget, float Padding)
{
	if (!Widget)
	{
		return;
	}
	if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(Widget->Slot))
	{
		const FMargin Margin(Padding);
		OverlaySlot->SetPadding(Margin);
	}
}

void UAuraWidgetFunctionLibrary::SetProgressBarImage(
	UProgressBar* ProgressBar,
	const FSlateBrush& FillImage
)
{
	const FSlateColor SlateColor(FColor::Transparent);
	const FSlateColorBrush BackgroundImage(SlateColor);
	FProgressBarStyle Style;
	Style.SetBackgroundImage(BackgroundImage);
	Style.SetFillImage(FillImage);
	ProgressBar->SetWidgetStyle(Style);
	ProgressBar->SetFillColorAndOpacity(FColor::White);
}

void UAuraWidgetFunctionLibrary::InterpolateProgressBarValue(
	UProgressBar* ProgressBar,
	const float TargetValue,
	const float DeltaTime,
	const float InterpSpeed
)
{
	if (ProgressBar->GetVisibility() == ESlateVisibility::Hidden)
	{
		return;
	}
	if (TargetValue == ProgressBar->GetPercent())
	{
		return;
	}
	ProgressBar->SetPercent(FMath::FInterpTo(ProgressBar->GetPercent(), TargetValue, DeltaTime, InterpSpeed));
}
