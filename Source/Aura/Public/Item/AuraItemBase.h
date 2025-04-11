// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "AuraItemTypes.h"
#include "GameFramework/Actor.h"
#include "AuraItemBase.generated.h"

class UGameplayEffect;

UCLASS()
class AURA_API AAuraItemBase : public AActor
{
	GENERATED_BODY()

public:
	AAuraItemBase();

	USkeletalMeshComponent* GetMesh() const;
	bool IsEquipped() const;
	void Equip(AActor* InOwner);
	void UnEquip(AActor* InOwner);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	EAuraItemType ItemType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	TSubclassOf<UGameplayEffect> EquipGameplayEffect;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> MeshComponent;

	UPROPERTY()
	FActiveGameplayEffectHandle EquippedHandle;
	bool bIsEquipped;
};
