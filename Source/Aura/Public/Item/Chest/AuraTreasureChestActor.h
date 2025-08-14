// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Actor/AuraPOI.h"
#include "Interaction/HighlightInterface.h"
#include "Interaction/SaveInterface.h"
#include "AuraTreasureChestActor.generated.h"


class AAuraTreasurePickup;
class AAuraPickupItemBase;
class UAuraLockComponent;

USTRUCT(BlueprintType)
struct FAuraLootDefinition
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AAuraPickupItemBase> ItemClass;
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
class AURA_API AAuraTreasureChestActor : public AAuraPOI, public ISaveInterface, public IHighlightInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAuraTreasureChestActor();
	virtual bool HandleInteract_Implementation(AActor* Player) override;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsUnlocked() const;
	UFUNCTION(BlueprintCallable)
	bool Unlock(AActor* Player);

	/** Start ISaveInterface **/
	virtual void LoadActor_Implementation() override;
	/** End ISaveInterface **/


protected:
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintImplementableEvent)
	void PlayUnlockEffect(const bool ForceOpen);
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Items")
	TArray<FAuraLootDefinition> LootDefinitions;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Items", meta=(MustImplement="/Script/Aura.SpawnEffectInterface"))
	TSubclassOf<AActor> RewardAnimatorClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Items")
	int32 GoldAmount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Items")
	float SpawnDistance = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Items")
	float SpawnRadius = 120.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Items")
	float HeightMultiplier = 1.f;

	UFUNCTION(BlueprintCallable)
	static FAuraLootInstance Pop(UPARAM(ref)
		TArray<FAuraLootInstance>& LootInstances);
};
