// Copyright Alien Shores


#include "Item/Component/AuraLockComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Player/InventoryActorInterface.h"


class UPlayerInventoryComponent;

UAuraLockComponent::UAuraLockComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UAuraLockComponent::IsPreconditionMet(const AActor* Player) const
{
	if (bUnlocked)
	{
		return false;
	}
	if (IsKeyRequiredToUnlock())
	{
		if (const UPlayerInventoryComponent* InventoryComponent = IInventoryActorInterface::GetInventoryComponent(Player))
		{
			return InventoryComponent->HasItemInInventory(KeyTag);
		}
	}
	else
	{
		return true;
	}
	return false;
}

bool UAuraLockComponent::TryUnlock(AActor* Player)
{
	if (bUnlocked)
	{
		return false;
	}
	if (IsKeyRequiredToUnlock())
	{
		if (UPlayerInventoryComponent* InventoryComponent = IInventoryActorInterface::GetInventoryComponent(Player))
		{
			bUnlocked = InventoryComponent->UseKey(KeyTag);
		}
	}
	else
	{
		bUnlocked = true;
	}
	if (bUnlocked)
	{
		if (UnlockSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, UnlockSound, GetOwner()->GetActorLocation());
		}
		OnUnlockDelegate.Broadcast();
	}
	return bUnlocked;
}

bool UAuraLockComponent::IsUnlocked() const
{
	return bUnlocked;
}

bool UAuraLockComponent::IsKeyRequiredToUnlock() const
{
	return KeyTag.IsValid();
}

FString UAuraLockComponent::GetUnlockText() const
{
	return IsKeyRequiredToUnlock() ? UnlockText : OpenText;
}
