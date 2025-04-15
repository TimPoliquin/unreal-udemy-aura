// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Fishing/AuraFishTypes.h"
#include "UObject/Interface.h"
#include "FishingComponentInterface.generated.h"

UENUM(BlueprintType)
enum class EFishingState : uint8
{
	None,
	Equipping,
	Equipped,
	Casting,
	Cast,
	Waiting,
	Lured,
	Biting,
	Reeling
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFishingStateChangedSignature, EFishingState, FishingState);


// This class does not need to be modified.
UINTERFACE()
class UFishingComponentInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IFishingComponentInterface
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
	void CastFishingRod();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Reel();
	virtual void SetupForFishing(const FVector& FishingDestination) = 0;
	virtual FOnFishingStateChangedSignature& GetOnFishingStateChangedDelegate() = 0;
	virtual void FishStateChanged(const EFishState& FishState) = 0;
	virtual void EndFishing() = 0;

	static void FishStateChanged(const UObject* Actor, const EFishState& FishState);
};
