// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Components/ActorComponent.h"
#include "Item/AuraItemTypes.h"
#include "PlayerInventoryComponent.generated.h"

class UGameplayEffect;
DECLARE_MULTICAST_DELEGATE(FOnEquipmentUseModeChangeSignature)


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AURA_API UPlayerInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
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

	FOnEquipmentUseModeChangeSignature OnUseWeapon;
	FOnEquipmentUseModeChangeSignature OnUseTool;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory|Equipment")
	TMap<EAuraEquipmentSlot, TSubclassOf<UGameplayEffect>> EquipGameplayEffectClasses;;
	UPROPERTY(VisibleAnywhere, Category="Inventory|Equipment")
	TMap<EAuraEquipmentSlot, FActiveGameplayEffectHandle> ActiveEquipGameplayEffects;

	UPROPERTY()
	TObjectPtr<AAuraItemBase> Weapon;
	UPROPERTY()
	TObjectPtr<AAuraItemBase> Tool;

private:
	AAuraItemBase* SpawnEquipment(const EAuraEquipmentSlot& Slot);
	void ClearEquipmentAbility(const EAuraEquipmentSlot& Slot);
	void ActivateEquipmentAbility(const EAuraEquipmentSlot& Slot);
};
