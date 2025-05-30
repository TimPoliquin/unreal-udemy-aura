// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraWidgetFunctionLibrary.generated.h"

class UInputAction;
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

	UFUNCTION(
		BlueprintCallable,
		Category = "AuraWidgetFunctionLibrary|ProgressBar"
	)
	static void InterpolateProgressBarValue(
		UProgressBar* ProgressBar,
		float TargetValue,
		float DeltaTime,
		float InterpSpeed
	);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AuraWidgetFunctionLibrary|Controller")
	static APlayerController* GetPlayerController(const AActor* Player);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AuraWidgetFunctionLibrary|Input")
	static bool IsInputActionBoundToKey(
		const UObject* WorldContextObject,
		int32 PlayerIndex,
		const UInputAction* InputAction,
		const FKey& Key
	);
};
