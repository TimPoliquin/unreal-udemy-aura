// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Actor/AuraPOI.h"
#include "Interaction/HighlightInterface.h"
#include "Interaction/SaveInterface.h"
#include "AuraTreasureChestActor.generated.h"


struct FOnAuraLockComponentUnlockPayload;
class AAuraTreasurePickup;
class AAuraPickupItemBase;
class UAuraLockComponent;

UENUM(BlueprintType)
enum class EAuraTreasureChestState : uint8
{
	Locked,
	Unlocked,
	Open
};

UENUM(BlueprintType)
enum class EAuraTreasureChestGrantMode : uint8
{
	Spawn,
	DirectToInventory
};

USTRUCT(BlueprintType)
struct FAuraLootDefinition
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Item"))
	FGameplayTag ItemTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = 1;
};

USTRUCT(BlueprintType)
struct FAuraLootInstance
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TWeakObjectPtr<AActor> LootItem;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FTransform SpawnTransform;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FTransform TargetTransform;
};

UCLASS()
class AURA_API AAuraTreasureChestActor : public AAuraPOI, public IHighlightInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAuraTreasureChestActor();
	virtual void HandleInteract_Implementation(AActor* Player) override;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsUnlocked() const;
	void HandleInitialState();

	/** Start ISaveInterface **/
	virtual void PostLoad_Implementation() override;
	/** End ISaveInterface **/


protected:
	virtual void BeginPlay() override;
	virtual bool IsPreconditionMet_Implementation(AActor* Player) const override;
	void Unlock(AActor* Player);
	void Open(AActor* Player);
	UFUNCTION(BlueprintCallable)
	void GrantRewards(AActor* Player);
	UFUNCTION(BlueprintImplementableEvent)
	void PlayUnlockEffect();
	UFUNCTION(BlueprintImplementableEvent)
	void PlayUnlockForcedEffect();
	UFUNCTION(BlueprintImplementableEvent)
	void PlayOpenEffect(AActor* Player);
	UFUNCTION(BlueprintImplementableEvent)
	void PlayOpenForcedEffect();
	UFUNCTION(BlueprintImplementableEvent)
	void GrantRewards_Spawn();
	UFUNCTION(BlueprintNativeEvent)
	void GrantRewards_DirectToInventory(AActor* Player);
	UFUNCTION(BlueprintNativeEvent)
	FTransform GetRewardInitialSpawnLocation() const;
	UFUNCTION(BlueprintCallable)
	TArray<FAuraLootInstance> InstantiateRewardActors();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PlaySpawnRewards(
		UPARAM(ref)
		TArray<FAuraLootInstance>& RewardActors
	);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlaySpawnReward(const FAuraLootInstance& RewardInstance);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UAuraLockComponent> LockComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category="Components")
	EAuraTreasureChestState State = EAuraTreasureChestState::Locked;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Items")
	TArray<FAuraLootDefinition> LootDefinitions;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Items")
	int32 GoldAmount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Items")
	EAuraTreasureChestGrantMode GrantMode = EAuraTreasureChestGrantMode::Spawn;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Items",
		meta=(MustImplement="/Script/Aura.SpawnEffectInterface", EditCondition="GrantMode == EAuraTreasureChestGrantMode::Spawn", EditConditionHides))
	TSubclassOf<AActor> RewardAnimatorClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Items", meta=(EditCondition="GrantMode == EAuraTreasureChestGrantMode::Spawn", EditConditionHides))
	float SpawnDistance = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Items", meta=(EditCondition="GrantMode == EAuraTreasureChestGrantMode::Spawn", EditConditionHides))
	float SpawnRadius = 120.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Items", meta=(EditCondition="GrantMode == EAuraTreasureChestGrantMode::Spawn", EditConditionHides))
	float HeightMultiplier = 1.f;

	UFUNCTION(BlueprintCallable)
	static FAuraLootInstance Pop(UPARAM(ref)
		TArray<FAuraLootInstance>& LootInstances);

private:
	UFUNCTION()
	void OnChestUnlocked(const FOnAuraLockComponentUnlockPayload& Payload);
};
