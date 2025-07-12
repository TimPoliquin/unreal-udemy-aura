// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AuraInteractionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UAuraInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IAuraInteractionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool OnInteract(AActor* Player);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnInteractionEnd(AActor* Player, const bool bIsCancelled);
};
