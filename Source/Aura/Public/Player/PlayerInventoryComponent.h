// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interaction/SavableInterface.h"
#include "Item/AuraItemTypes.h"
#include "AuraInventoryEvents.h"
#include "PlayerInventoryComponent.generated.h"

class AAuraFishingRod;
class UGameplayEffect;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryFullSignature, const FGameplayTag&, ItemType);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryItemCountChangedSignature, const FOnInventoryItemCountChangedPayload&, Payload);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AURA_API UPlayerInventoryComponent : public UActorComponent, public ISavableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="PlayerInventoryComponent|Utils")
	static UPlayerInventoryComponent* GetPlayerInventoryComponent(const AActor* InActor);

	UPlayerInventoryComponent();

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

	/** ISavableInterface start **/
	virtual void FromSaveData(const UAuraSaveGame* SaveData) override;
	virtual void ToSaveData(UAuraSaveGame* SaveData) const override;
	/** ISavableInterface end **/
	TArray<FAuraItemInventoryEntry> GetInventory() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	int32 MaxItems = 25;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	TArray<FAuraItemInventoryEntry> Inventory;

private:
	bool UseItem(const FGameplayTag& ItemTag, const EAuraItemCategory& ItemCategory);
};
