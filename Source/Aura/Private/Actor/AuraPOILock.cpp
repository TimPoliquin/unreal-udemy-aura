// Copyright Alien Shores


#include "Actor/AuraPOILock.h"

#include "Actor/AuraLockedInterface.h"
#include "Aura/Aura.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/InventoryActorInterface.h"
#include "Player/PlayerInventoryComponent.h"


AAuraPOILock::AAuraPOILock()
{
	PrimaryActorTick.bCanEverTick = false;
	LockMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lock Mesh Component"));
	LockMeshComponent->SetupAttachment(GetRootComponent());
	LockMeshComponent->SetRenderCustomDepth(false);
}

void AAuraPOILock::HighlightActor_Implementation()
{
	if (!bUnlocked)
	{
		LockMeshComponent->SetRenderCustomDepth(true);
	}
}

void AAuraPOILock::UnHighlightActor_Implementation()
{
	LockMeshComponent->SetRenderCustomDepth(false);
}

void AAuraPOILock::BeginPlay()
{
	Super::BeginPlay();
	if (bUnlocked)
	{
		DisablePOI();
		return;
	}
	if (InteractionWidget && InteractionWidget->GetWidget())
	{
		InitializeInteractionWidgetSettings(InteractionWidget->GetWidget(), InteractText);
	}
	if (PreconditionWidget && PreconditionWidget->GetWidget())
	{
		InitializePreconditionWidgetSettings(PreconditionWidget->GetWidget(), RuneIcon);
	}
	LockMeshComponent->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
}

bool AAuraPOILock::IsPreconditionMet_Implementation(AActor* Player) const
{
	if (bUnlocked)
	{
		return false;
	}
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
	if (bUnlocked)
	{
		return true;
	}
	if (UPlayerInventoryComponent* InventoryComponent = IInventoryActorInterface::GetInventoryComponent(Player))
	{
		if (InventoryComponent->UseKey(KeyTag))
		{
			bUnlocked = true;
			if (UnlockSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, UnlockSound, GetActorLocation());
			}
			for (AActor* Gate : Gates)
			{
				IAuraLockedInterface::Unlock(Gate);
			}
			PlayUnlockEffect(Player);
			DisablePOI();
			UnHighlightActor(this);
			return true;
		}
	}
	return false;
}
