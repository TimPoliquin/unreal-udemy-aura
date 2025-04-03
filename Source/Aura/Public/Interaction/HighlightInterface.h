// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HighlightInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UHighlightInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IHighlightInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HighlightActor();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UnHighlightActor();

	static void HighlightActor(UObject* Actor);
	static void UnHighlightActor(UObject* Actor);
	static bool IsHighlightActor(const UObject* Actor);
};
