// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	void AddToXP(int32 InXP);
	UFUNCTION(BlueprintNativeEvent)
	int32 GetXP();
	UFUNCTION(BlueprintNativeEvent)
	int32 FindLevelForXP(const int32 CurrentXP) const;


	UFUNCTION(BlueprintNativeEvent)
	FAuraLevelUpRewards GetLevelUpRewards(const int32 CurrentLevel) const;
	UFUNCTION(BlueprintNativeEvent)
	void ApplyLevelUpRewards(const int32 LevelIncrement, const FAuraLevelUpRewards& InLevelUpRewards);
	UFUNCTION(BlueprintNativeEvent)
	void SpendAttributePoints(int32 SpentPoints);
	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePoints() const;
	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPoints() const;

	UFUNCTION(BlueprintNativeEvent)
	void LevelUp();

	static void AddToXP(AActor* Actor, int32 InXP);
	static bool ImplementsPlayerInterface(const UObject* Object);
	static int32 GetAttributePoints(const UObject* Object);
	static void SpendAttributePoints(UObject* Object, int32 SpentPoints);
};
