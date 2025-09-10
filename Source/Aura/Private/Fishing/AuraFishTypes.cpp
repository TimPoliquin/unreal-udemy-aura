#include "Fishing/AuraFishTypes.h"

#include "Fishing/AuraFishCatch.h"
#include "Player/AuraInventoryComponent.h"

void UAuraFishCatch::AddToPlayerInventory(AActor* Player)
{
	if (UAuraInventoryComponent* InventoryComponent = UAuraInventoryComponent::Get(Player))
	{
		InventoryComponent->AddToInventory(FishType);
	}
}
