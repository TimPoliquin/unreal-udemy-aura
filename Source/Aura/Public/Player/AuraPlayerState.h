// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "InventoryActorInterface.h"
#include "AbilitySystem/AttributeChangeDelegates.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "GameFramework/PlayerState.h"
#include "Interaction/SavableInterface.h"
#include "Item/AuraItemTypes.h"
#include "AuraPlayerState.generated.h"

class UAuraAttributeSet;
class UAuraAbilitySystemComponent;
class UPlayerInventoryComponent;
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
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface, public ISavableInterface
{
	GENERATED_BODY()

public:
	AAuraPlayerState();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UAttributeSet* GetAttributeSet() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetCharacterLevel() const;
	void AddToLevel(const int32 AddLevel);
	int32 GetXP() const;
	void AddToXP(const int32 InXP);
	int32 GetAttributePoints() const;
	void AddAttributePoints(const int32 InAttributePoints);
	int32 GetSpellPoints() const;
	void AddSpellPoints(const int32 InSpellPoints);

	float GetXPToNextLevelPercentage() const;
	int32 FindLevelByXP(const int32 InXP) const;
	FAuraLevelUpRewards GetLevelUpRewards(int32 int32) const;
	virtual void FromSaveData(const UAuraSaveGame* SaveData) override;
	virtual void ToSaveData(UAuraSaveGame* SaveData) const override;
	void InitializeAbilityActorInfo();

	FOnPlayerStatChangedSignature OnXPChangeDelegate;
	FOnPlayerStatChangedSignature OnLevelChangeDelegate;
	FOnPlayerStatChangedSignature OnLevelInitializedDelegate;
	FOnPlayerStatChangedSignature OnAttributePointsChangeDelegate;
	FOnPlayerStatChangedSignature OnSpellPointsChangeDelegate;
	FOnPlayerInventoryChangedSignature OnPlayerInventoryChangedDelegate;

protected:
	UPROPERTY(Replicated)
	TObjectPtr<UAuraAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(Replicated)
	TObjectPtr<UAuraAttributeSet> AttributeSet;

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

private:
	void InitializeLevel(const int32 InLevel);
	void SetLevel(const int32 NewLevel);
	void SetXP(const int32 InXP);
	void SetAttributePoints(const int32 InAttributePoints);
	void SetSpellPoints(const int32 InSpellPoints);

	UFUNCTION()
	void OnRep_Level(int32 OldLevel) const;
	UFUNCTION()
	void OnRep_XP(int32 OldXP) const;
	UFUNCTION()
	void OnRep_AttributePoints(int32 InAttributePoints) const;
	UFUNCTION()
	FORCEINLINE void OnRep_SpellPoints(int32 InSpellPoints) const;
};
