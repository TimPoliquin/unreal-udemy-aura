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
DECLARE_MULTICAST_DELEGATE(FOnEquipmentUseModeChangeSignature)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnEquipmentAnimationRequestSignature,
	EAuraEquipmentSlot,
	EquipmentSlot,
	const FGameplayTag&,
	EquippedItem
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnEquipmentAnimationCompleteSignature,
	EAuraEquipmentSlot,
	EquipmentSlot,
	const FGameplayTag&,
	EquippedItem
);

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
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasToolEquipped(const FGameplayTag& ItemType) const;

	bool IsUsingTool() const;
	bool IsUsingWeapon() const;
	UFUNCTION(BlueprintCallable, Category="Item|Equipment")
	void UseTool();
	UFUNCTION(BlueprintCallable, Category="Item|Equipment")
	void UseWeapon();
	UFUNCTION(BlueprintCallable, Category="Item|Equipment")
	void UseNothing();
	void Equip(const EAuraEquipmentSlot& Slot, const FGameplayTag& ItemType);
	USkeletalMeshComponent* GetWeapon() const;
	UFUNCTION(BlueprintCallable, Category="Item|Equipment")
	FGameplayTag GetToolType() const;
	UFUNCTION(BlueprintCallable, Category="Item|Equipment")
	FGameplayTag GetWeaponType() const;
	UFUNCTION(BlueprintCallable, Category="Item|Equipment")
	AAuraFishingRod* GetFishingRod() const;
	UFUNCTION(BlueprintCallable, Category="Item|Equipment")
	FGameplayTag GetEquippedItem(const EAuraEquipmentSlot Slot) const;
	EAuraEquipmentUseMode GetEquipmentUseMode() const;
	void PlayEquipAnimation(EAuraEquipmentSlot Slot) const;
	UFUNCTION(BlueprintCallable, Category="Item")
	int32 AddToInventory(const FGameplayTag& ItemType, int32 Count = 1);
	UFUNCTION(BlueprintCallable, Category="Item")
	bool UseConsumable(const FGameplayTag& ItemType);
	UFUNCTION(BlueprintCallable, Category="Item")
	bool UseKey(const FGameplayTag& ItemType);

	FOnEquipmentUseModeChangeSignature OnUseWeapon;
	FOnEquipmentUseModeChangeSignature OnUseTool;
	UPROPERTY(BlueprintAssignable, Category="Item|Equipment")
	FOnEquipmentAnimationRequestSignature OnEquipmentAnimationRequest;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnEquipmentAnimationCompleteSignature OnEquipmentAnimationCompleteDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnInventoryItemCountChangedSignature OnInventoryItemCountChangedDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnInventoryFullSignature OnInventoryFullDelegate;

	/** ISavableInterface start **/
	virtual void FromSaveData(const UAuraSaveGame* SaveData) override;
	virtual void ToSaveData(UAuraSaveGame* SaveData) const override;
	/** ISavableInterface end **/

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	int32 MaxItems = 16;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	TArray<FAuraItemInventoryEntry> Inventory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory|Equipment")
	EAuraEquipmentUseMode EquipmentUseMode = EAuraEquipmentUseMode::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory|Equipment")
	TMap<EAuraEquipmentSlot, FGameplayTag> EquipmentSlots;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory|Equipment")
	TMap<EAuraEquipmentSlot, FName> EquipmentSocketNames;

	UPROPERTY()
	TObjectPtr<AAuraEquipmentBase> Weapon;
	UPROPERTY()
	TObjectPtr<AAuraEquipmentBase> Tool;

private:
	AAuraEquipmentBase* SpawnEquipment(const EAuraEquipmentSlot& Slot);
	bool UseItem(const FGameplayTag& ItemTag, const EAuraItemCategory& ItemCategory);
};
