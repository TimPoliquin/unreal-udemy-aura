// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AuraAbilitySystemInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UAuraAbilitySystemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IAuraAbilitySystemInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	int32 GetCharacterLevel() const;
	static int32 GetCharacterLevel(const UObject* Character);
	static bool IsAbilitySystemReady(AActor* Actor);
};
