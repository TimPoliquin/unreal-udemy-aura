// Copyright Alien Shores


#include "Item/Pickup/AuraPickupItemInventory.h"

#include "Interaction/PlayerInterface.h"
#include "Player/AuraInventoryComponent.h"


AAuraPickupItemInventory::AAuraPickupItemInventory()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAuraPickupItemInventory::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraPickupItemInventory::ActivatePickup(AActor* PickupActor)
{
	if (UAuraInventoryComponent* InventoryComponent = UAuraInventoryComponent::Get(PickupActor))
	{
		const int32 AmountAdded = InventoryComponent->AddToInventory(ItemType, Count);
		if (AmountAdded > 0)
		{
			PlayPickupEffect(PickupActor, AmountAdded == Count);
		}
	}
}

void AAuraPickupItemInventory::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                              const FHitResult& SweepResult)
{
	if (IsValid(OtherActor) && OtherActor->Implements<UPlayerInterface>())
	{
		ActivatePickup(OtherActor);
	}
}
