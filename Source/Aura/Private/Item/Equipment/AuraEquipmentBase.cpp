// Copyright Alien Shores


#include "Item/Equipment/AuraEquipmentBase.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"


// Sets default values
AAuraEquipmentBase::AAuraEquipmentBase()
{
	PrimaryActorTick.bCanEverTick = false;
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(GetRootComponent());
}

USkeletalMeshComponent* AAuraEquipmentBase::GetMesh() const
{
	return MeshComponent;
}

bool AAuraEquipmentBase::IsEquipped() const
{
	return bIsEquipped;
}

void AAuraEquipmentBase::Equip(AActor* InOwner)
{
	if (IsEquipped())
	{
		return;
	}
	bIsEquipped = true;
	if (EquipGameplayEffect)
	{
		EquippedHandle = UAuraAbilitySystemLibrary::ApplyBasicGameplayEffect(InOwner, EquipGameplayEffect);
	}
}

void AAuraEquipmentBase::UnEquip(AActor* InOwner)
{
	if (!IsEquipped())
	{
		return;
	}
	bIsEquipped = false;
	if (EquippedHandle.IsValid())
	{
		UAuraAbilitySystemLibrary::RemoveGameplayEffect(InOwner, EquippedHandle);
		EquippedHandle.Invalidate();
	}
}
