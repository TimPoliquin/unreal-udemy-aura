// Copyright Alien Shores


#include "Item/Pickup/AuraPickupItemInventory.h"

#include "Components/SphereComponent.h"
#include "Player/InventoryActorInterface.h"
#include "Player/PlayerInventoryComponent.h"


AAuraPickupItemInventory::AAuraPickupItemInventory()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAuraPickupItemInventory::BeginPlay()
{
	Super::BeginPlay();
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AAuraPickupItemInventory::OnBeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AAuraPickupItemInventory::OnEndOverlap);
}

void AAuraPickupItemInventory::ActivatePickup(AActor* PickupActor)
{
	if (UPlayerInventoryComponent* InventoryComponent = IInventoryActorInterface::GetInventoryComponent(PickupActor))
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
	if (IsValid(OtherActor) && OtherActor->Implements<UInventoryActorInterface>())
	{
		OnPickupAvailable(OtherActor);
	}
}

void AAuraPickupItemInventory::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IsValid(OtherActor) && OtherActor->Implements<UInventoryActorInterface>())
	{
		OnPickupUnavailable(OtherActor);
	}
}
