// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "Fishing/AuraFishTypes.h"
#include "UObject/Interface.h"
#include "FishingComponentInterface.generated.h"

class UGameplayAbility;

UENUM(BlueprintType)
enum class EFishingState : uint8
{
	None,
	Preparing,
	Equipping,
	Equipped,
	Casting,
	Cast,
	Waiting,
	Lured,
	Biting,
	Reeling,
	Caught
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

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool HasFishingRod();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool HasFishingRodEquipped();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EquipFishingRod();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CastFishingRod();
	virtual bool IsFishing() const = 0;
	virtual void SetupForFishing(const FVector& FishingDestination) = 0;
	virtual FOnFishingStateChangedSignature& GetOnFishingStateChangedDelegate() = 0;
	virtual void FishStateChanged(const EFishState& FishState) = 0;
	virtual float GetRarityMultiplier(const EFishRarity& Rarity) const = 0;
	virtual TArray<EFishTag> GetFishingTags() const = 0;
	virtual void Reel() = 0;
	virtual void EndFishing() = 0;
	virtual void PrepareForContinue() = 0;

	static void FishStateChanged(const UObject* Actor, const EFishState& FishState);
	static float GetRarityMultiplier(const UObject* Actor, const EFishRarity& Rarity);
	static bool IsFishing(const UObject* Actor);
	static TArray<EFishTag> GetFishingTags(const UObject* Actor);
};
