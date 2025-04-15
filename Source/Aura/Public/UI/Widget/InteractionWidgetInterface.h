// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionWidgetInterface.generated.h"

class UUserWidget;

// This class does not need to be modified.
UINTERFACE()
class UInteractionWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IInteractionWidgetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Show();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Hide(bool bAnimate = true);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetInteractionKey(const FString& Key);

	static void Show(UUserWidget* Widget);
	static void Hide(UUserWidget* Widget, const bool bAnimate = true);
};
