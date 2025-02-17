// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraWidgetFunctionLibrary.generated.h"

class UWidget;
class UProgressBar;
class UOverlaySlot;
class USizeBox;
/**
 * 
 */
UCLASS()
class AURA_API UAuraWidgetFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "AuraWidgetFunctionLibrary")
	static void SetBoxSizeOverride(USizeBox* SizeBox, float Height, float Width);

	UFUNCTION(BlueprintCallable, Category = "AuraWidgetFunctionLibrary")
	static void SetWidgetPadding(UWidget* Widget, float Padding);

	UFUNCTION(
		BlueprintCallable,
		Category = "AuraWidgetFunctionLibrary|ProgressBar",
		meta = (DefaultColor = "0.0, 0.0, 0.0, 0.0")
	)
	static void SetProgressBarImage(
		UProgressBar* ProgressBar,
		const FSlateBrush& FillImage
	);
};
