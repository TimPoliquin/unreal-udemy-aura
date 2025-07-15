// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Item/AuraItemTypes.h"
#include "GameFramework/Actor.h"
#include "AuraEquipmentBase.generated.h"

class UGameplayEffect;

UCLASS()
class AURA_API AAuraEquipmentBase : public AActor
{
	GENERATED_BODY()

public:
	AAuraEquipmentBase();

	USkeletalMeshComponent* GetMesh() const;
	bool IsEquipped() const;
	virtual void Equip(AActor* InOwner);
	virtual void UnEquip(AActor* InOwner);
	FGameplayTag GetItemType() const { return ItemType; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item", meta=(Categories="Item.Type"))
	FGameplayTag ItemType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	TSubclassOf<UGameplayEffect> EquipGameplayEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> MeshComponent;

private:
	UPROPERTY()
	FActiveGameplayEffectHandle EquippedHandle;
	bool bIsEquipped;
};
