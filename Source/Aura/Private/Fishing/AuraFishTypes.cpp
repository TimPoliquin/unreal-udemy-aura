#include "Fishing/AuraFishTypes.h"

#include "Fishing/AuraFishCatch.h"
#include "Player/InventoryActorInterface.h"
#include "Player/PlayerInventoryComponent.h"

void UAuraFishCatch::AddToPlayerInventory(AActor* Player)
{
	if (UPlayerInventoryComponent* InventoryComponent = IInventoryActorInterface::GetInventoryComponent(Player))
	{
		InventoryComponent->AddToInventory(FishType);
	}
}
