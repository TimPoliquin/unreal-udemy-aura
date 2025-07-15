// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AuraGateInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UAuraGateInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IAuraGateInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Unlock();

	static void Unlock(UObject* Gate);
};
