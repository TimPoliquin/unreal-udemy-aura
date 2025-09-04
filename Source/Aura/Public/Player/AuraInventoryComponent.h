// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/AuraItemTypes.h"
#include "AuraInventoryEvents.h"
#include "Game/Save/SaveableInterface.h"
#include "AuraInventoryComponent.generated.h"

class AAuraFishingRod;
class UGameplayEffect;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryFullSignature, const FGameplayTag&, ItemType);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryItemCountChangedSignature, const FOnInventoryItemCountChangedPayload&, Payload);

USTRUCT(BlueprintType)
struct AURA_API FAuraInventoryComponentSaveData
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	int32 MaxItems = 0;
	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	TArray<FAuraItemInventoryEntry> InventoryItems;
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AURA_API UAuraInventoryComponent : public UActorComponent, public ISaveableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="PlayerInventoryComponent|Utils")
	static UAuraInventoryComponent* Get(const UObject* InObject);

	UAuraInventoryComponent();

	/** Start ISaveableInterface **/
	virtual TArray<uint8> SaveData_Implementation() override;
	virtual bool LoadData_Implementation(const TArray<uint8>& Data) override;
	virtual bool ShouldSave_Implementation() const override;
	virtual bool ShouldLoadTransform_Implementation() const override { return false; }
	virtual bool ShouldAutoSpawn_Implementation() const override { return false; }
	virtual FString GetSaveID_Implementation() const override;
	/** Start ISaveableInterface **/


	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasItemInInventory(const FGameplayTag& ItemType) const;
	UFUNCTION(BlueprintCallable, Category="Item")
	int32 AddToInventory(const FGameplayTag& ItemType, int32 Count = 1);
	UFUNCTION(BlueprintCallable, Category="Item")
	bool UseConsumable(const FGameplayTag& ItemType);
	UFUNCTION(BlueprintCallable, Category="Item")
	bool UseKey(const FGameplayTag& ItemType);

	UPROPERTY(BlueprintAssignable)
	FOnInventoryItemCountChangedSignature OnInventoryItemCountChangedDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnInventoryFullSignature OnInventoryFullDelegate;

	TArray<FAuraItemInventoryEntry> GetInventory() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	int32 MaxItems = 25;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	TArray<FAuraItemInventoryEntry> Inventory;

private:
	FString SaveID;
	bool UseItem(const FGameplayTag& ItemTag, const EAuraItemCategory& ItemCategory);
	TArray<uint8> SerializeComponentData() const;
	bool DeserializeComponentData(const TArray<uint8>& Data);
};
