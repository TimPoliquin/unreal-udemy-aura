// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Equipment/AuraEquipmentEvents.h"
#include "Game/Save/SaveableInterface.h"
#include "Item/AuraItemTypes.h"
#include "AuraPlayerEquipmentComponent.generated.h"

class AAuraFishingRod;
class AAuraEquipmentBase;

USTRUCT(BlueprintType)
struct AURA_API FAuraPlayerEquipmentComponentSaveData
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, Category="Save Data")
	EAuraEquipmentUseMode EquipmentUseMode = EAuraEquipmentUseMode::None;
	UPROPERTY(BlueprintReadWrite, Category="Save Data")
	TMap<EAuraEquipmentSlot, FGameplayTag> EquipmentSlots;
};

DECLARE_MULTICAST_DELEGATE(FOnEquipmentUseModeChangeSignature)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnEquipmentAnimationCompleteSignature,
	EAuraEquipmentSlot,
	EquipmentSlot,
	const FGameplayTag&,
	EquippedItem
);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AURA_API UAuraPlayerEquipmentComponent : public UActorComponent, public ISaveableInterface
{
	GENERATED_BODY()

public:
	UAuraPlayerEquipmentComponent();

	/** Start ISaveableInterface **/
	virtual TArray<uint8> SaveData_Implementation() override;
	virtual bool LoadData_Implementation(const TArray<uint8>& Data) override;
	virtual bool ShouldSave_Implementation() const override { return true; }
	virtual bool ShouldLoadTransform_Implementation() const override { return false; }
	virtual bool ShouldAutoSpawn_Implementation() const override { return false; }
	virtual FString GetSaveID_Implementation() const override;
	/** End ISaveableInterface **/

	FOnEquipmentUseModeChangeSignature OnUseWeapon;
	FOnEquipmentUseModeChangeSignature OnUseTool;
	UPROPERTY(BlueprintAssignable)
	FAuraEquipmentDelegateSignature OnEquipmentAnimationRequest;
	UPROPERTY(BlueprintAssignable)
	FAuraEquipmentDelegateSignature OnEquipmentAnimationCompleteDelegate;

	void InitializeEquipment();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasToolEquipped(const FGameplayTag& ItemType) const;


	bool IsUsingTool() const;
	bool IsUsingWeapon() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UseEquipment(const EAuraEquipmentSlot Slot);
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
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void EquipAnimationCompleted(const EAuraEquipmentSlot Slot);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory|Equipment")
	EAuraEquipmentUseMode EquipmentUseMode = EAuraEquipmentUseMode::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory|Equipment")
	TMap<EAuraEquipmentSlot, FGameplayTag> EquipmentSlots;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory|Equipment")
	TMap<EAuraEquipmentSlot, FName> EquipmentSocketNames;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Debug")
	FString SaveID;

private:
	USkeletalMeshComponent* GetCharacterMesh() const;
	UPROPERTY()
	TObjectPtr<AAuraEquipmentBase> Weapon;
	UPROPERTY()
	TObjectPtr<AAuraEquipmentBase> Tool;

	AAuraEquipmentBase* SpawnEquipment(const EAuraEquipmentSlot& Slot);

	TArray<uint8> SerializeComponentData() const;
	bool DeserializeComponentData(const TArray<uint8>& Data);
};
