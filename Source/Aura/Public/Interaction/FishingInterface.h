// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FishingInterface.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFishingRodEquippedSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFishingRodCastSignature);

// This class does not need to be modified.
UINTERFACE()
class UFishingInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IFishingInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool HasFishingRod();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool HasFishingRodEquipped();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EquipFishingRod();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CastFishingRod(const FVector& FishingLocation);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EndFishing();

	virtual FOnFishingRodEquippedSignature& GetOnFishingRodEquippedDelegate() = 0;
	virtual FOnFishingRodCastSignature& GetOnFishingRodCastDelegate() = 0;
};
