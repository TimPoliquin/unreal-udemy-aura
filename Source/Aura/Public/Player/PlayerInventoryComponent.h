// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Components/ActorComponent.h"
#include "Item/AuraItemTypes.h"
#include "PlayerInventoryComponent.generated.h"

class AAuraFishingRod;
class UGameplayEffect;
DECLARE_MULTICAST_DELEGATE(FOnEquipmentUseModeChangeSignature)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnEquipmentAnimationRequestSignature,
	EAuraEquipmentSlot,
	EquipmentSlot,
	EAuraItemType,
	EquippedItem
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnEquipmentAnimationCompleteSignature,
	EAuraEquipmentSlot,
	EquipmentSlot,
	EAuraItemType,
	EquippedItem
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnItemAddedSignature,
	const EAuraItemType&,
	ItemType,
	const int32,
	Count,
	const bool,
	bAddedAll
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryFullSignature, const EAuraItemType&, ItemType);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AURA_API UPlayerInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="PlayerInventoryComponent|Utils")
	static UPlayerInventoryComponent* GetPlayerInventoryComponent(const AActor* InActor);

	UPlayerInventoryComponent();

	bool HasItemInInventory(const EAuraItemType& ItemType) const;
	bool HasToolEquipped(const EAuraItemType& ItemType) const;

	bool IsUsingTool() const;
	bool IsUsingWeapon() const;
	UFUNCTION(BlueprintCallable, Category="Item|Equipment")
	void UseTool();
	UFUNCTION(BlueprintCallable, Category="Item|Equipment")
	void UseWeapon();
	UFUNCTION(BlueprintCallable, Category="Item|Equipment")
	void UseNothing();
	void Equip(const EAuraEquipmentSlot& Slot, const EAuraItemType& ItemType);
	USkeletalMeshComponent* GetWeapon() const;
	UFUNCTION(BlueprintCallable, Category="Item|Equipment")
	EAuraItemType GetToolType() const;
	UFUNCTION(BlueprintCallable, Category="Item|Equipment")
	EAuraItemType GetWeaponType() const;
	UFUNCTION(BlueprintCallable, Category="Item|Equipment")
	AAuraFishingRod* GetFishingRod() const;
	UFUNCTION(BlueprintCallable, Category="Item|Equipment")
	EAuraItemType GetEquippedItem(const EAuraEquipmentSlot Slot) const;
	EAuraEquipmentUseMode GetEquipmentUseMode() const;
	void PlayEquipAnimation(EAuraEquipmentSlot Slot) const;
	UFUNCTION(BlueprintCallable, Category="Item")
	int32 AddToInventory(const EAuraItemType& ItemType, const int32 Count = 1);

	FOnEquipmentUseModeChangeSignature OnUseWeapon;
	FOnEquipmentUseModeChangeSignature OnUseTool;
	UPROPERTY(BlueprintAssignable, Category="Item|Equipment")
	FOnEquipmentAnimationRequestSignature OnEquipmentAnimationRequest;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnEquipmentAnimationCompleteSignature OnEquipmentAnimationCompleteDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnItemAddedSignature OnItemAddedDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnInventoryFullSignature OnInventoryFullDelegate;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	int32 MaxItems = 16;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	TArray<FAuraItemInventoryEntry> Inventory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory|Equipment")
	EAuraEquipmentUseMode EquipmentUseMode = EAuraEquipmentUseMode::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory|Equipment")
	TMap<EAuraEquipmentSlot, EAuraItemType> EquipmentSlots;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory|Equipment")
	TMap<EAuraEquipmentSlot, FName> EquipmentSocketNames;

	UPROPERTY()
	TObjectPtr<AAuraItemBase> Weapon;
	UPROPERTY()
	TObjectPtr<AAuraItemBase> Tool;

private:
	AAuraItemBase* SpawnEquipment(const EAuraEquipmentSlot& Slot);
};
