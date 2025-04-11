// Copyright Alien Shores


#include "Item/AuraItemBase.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"


// Sets default values
AAuraItemBase::AAuraItemBase()
{
	PrimaryActorTick.bCanEverTick = false;
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(GetRootComponent());
}

USkeletalMeshComponent* AAuraItemBase::GetMesh() const
{
	return MeshComponent;
}

bool AAuraItemBase::IsEquipped() const
{
	return bIsEquipped;
}

void AAuraItemBase::Equip(AActor* InOwner)
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

void AAuraItemBase::UnEquip(AActor* InOwner)
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
