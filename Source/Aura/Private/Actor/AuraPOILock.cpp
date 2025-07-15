// Copyright Alien Shores


#include "Actor/AuraPOILock.h"

#include "Actor/AuraGateInterface.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/InventoryActorInterface.h"
#include "Player/PlayerInventoryComponent.h"


AAuraPOILock::AAuraPOILock()
{
	PrimaryActorTick.bCanEverTick = false;
	LockMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lock Mesh Component"));
	LockMeshComponent->SetupAttachment(GetRootComponent());
}

void AAuraPOILock::BeginPlay()
{
	Super::BeginPlay();
	if (InteractionWidget && InteractionWidget->GetWidget())
	{
		InitializeInteractionWidgetSettings(InteractionWidget->GetWidget(), InteractText);
	}
	if (PreconditionWidget && PreconditionWidget->GetWidget())
	{
		InitializePreconditionWidgetSettings(PreconditionWidget->GetWidget(), RuneColor, RuneIcon);
	}
}

bool AAuraPOILock::IsPreconditionMet_Implementation(AActor* Player) const
{
	if (const UPlayerInventoryComponent* InventoryComponent = IInventoryActorInterface::GetInventoryComponent(Player))
	{
		return InventoryComponent->HasItemInInventory(KeyTag);
	}
	return false;
}

bool AAuraPOILock::HandleInteract_Implementation(AActor* Player)
{
	return Unlock(Player);
}

bool AAuraPOILock::Unlock(AActor* Player)
{
	if (UPlayerInventoryComponent* InventoryComponent = IInventoryActorInterface::GetInventoryComponent(Player))
	{
		if (InventoryComponent->UseKey(KeyTag))
		{
			if (UnlockSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, UnlockSound, GetActorLocation());
			}
			IAuraGateInterface::Unlock(Gate);
			PlayUnlockEffect(Player);
			DisablePOI();
			return true;
		}
	}
	return false;
}
