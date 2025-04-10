// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/AttributeChangeDelegates.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "GameFramework/PlayerState.h"
#include "Item/AuraItemTypes.h"
#include "AuraPlayerState.generated.h"
class UAuraSaveGame;
class ULevelUpInfo;


class UAttributeSet;
class UAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnPlayerInventoryChangedSignature,
	TArray<FAuraItemInventoryEntry>,
	NewInventory
);

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAuraPlayerState();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	UAttributeSet* GetAttributeSet() const;
	FORCEINLINE int32 GetCharacterLevel() const
	{
		return Level;
	}

	FORCEINLINE void InitializeLevel(const int32 InLevel)
	{
		Level = InLevel;
		OnLevelInitializedDelegate.Broadcast(Level);
	}

	FORCEINLINE void SetLevel(const int32 NewLevel)
	{
		Level = NewLevel;
		OnLevelChangeDelegate.Broadcast(Level);
	}

	FORCEINLINE void AddToLevel(const int32 AddLevel)
	{
		SetLevel(Level + AddLevel);
	}

	FORCEINLINE int32 GetXP() const
	{
		return XP;
	}

	FORCEINLINE void SetXP(const int32 InXP)
	{
		XP = InXP;
		OnXPChangeDelegate.Broadcast(XP);
	}

	FORCEINLINE void AddToXP(const int32 InXP)
	{
		SetXP(InXP + XP);
	}

	FORCEINLINE int32 GetAttributePoints() const
	{
		return AttributePoints;
	}

	FORCEINLINE void SetAttributePoints(const int32 InAttributePoints)
	{
		AttributePoints = InAttributePoints;
		OnAttributePointsChangeDelegate.Broadcast(AttributePoints);
	}

	FORCEINLINE void AddAttributePoints(const int32 InAttributePoints)
	{
		SetAttributePoints(AttributePoints + InAttributePoints);
	}

	FORCEINLINE int32 GetSpellPoints() const
	{
		return SpellPoints;
	}

	FORCEINLINE void SetSpellPoints(const int32 InSpellPoints)
	{
		SpellPoints = InSpellPoints;
		OnSpellPointsChangeDelegate.Broadcast(SpellPoints);
	}

	FORCEINLINE void AddSpellPoints(const int32 InSpellPoints)
	{
		SetSpellPoints(SpellPoints + InSpellPoints);
	}

	float GetXPToNextLevelPercentage() const;
	int32 FindLevelByXP(const int32 InXP) const;
	FAuraLevelUpRewards GetLevelUpRewards(int32 int32) const;
	void FromSaveData(const UAuraSaveGame* SaveData);
	void ToSaveData(UAuraSaveGame* SaveData) const;

	FOnPlayerStatChangedSignature OnXPChangeDelegate;
	FOnPlayerStatChangedSignature OnLevelChangeDelegate;
	FOnPlayerStatChangedSignature OnLevelInitializedDelegate;
	FOnPlayerStatChangedSignature OnAttributePointsChangeDelegate;
	FOnPlayerStatChangedSignature OnSpellPointsChangeDelegate;
	FOnPlayerInventoryChangedSignature OnPlayerInventoryChangedDelegate;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo;
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level)
	int32 Level = 1;
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_XP)
	int32 XP = 0;
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_AttributePoints)
	int32 AttributePoints;
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_SpellPoints)
	int32 SpellPoints;

	UFUNCTION()
	FORCEINLINE void OnRep_Level(int32 OldLevel) const
	{
		OnLevelChangeDelegate.Broadcast(Level);
	}

	UFUNCTION()
	FORCEINLINE void OnRep_XP(int32 OldXP) const
	{
		OnXPChangeDelegate.Broadcast(XP);
	}

	UFUNCTION()
	FORCEINLINE void OnRep_AttributePoints(int32 InAttributePoints) const
	{
		OnAttributePointsChangeDelegate.Broadcast(AttributePoints);
	}

	UFUNCTION()
	FORCEINLINE void OnRep_SpellPoints(int32 InSpellPoints) const
	{
		OnSpellPointsChangeDelegate.Broadcast(SpellPoints);
	}
};
