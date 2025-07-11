// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "PlayerInventoryComponent.h"
#include "UObject/Interface.h"
#include "InventoryActorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UInventoryActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IInventoryActorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UPlayerInventoryComponent* GetInventoryComponent() const;

	static UPlayerInventoryComponent* GetInventoryComponent(const UObject* Object);
};
