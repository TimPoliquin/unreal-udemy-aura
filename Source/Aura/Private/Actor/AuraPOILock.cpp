// Copyright Alien Shores


#include "Actor/AuraPOILock.h"

#include "Actor/AuraLockedInterface.h"
#include "Aura/Aura.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraInventoryComponent.h"


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

void AAuraPOILock::PostLoad_Implementation()
{
	if (bUnlocked)
	{
		DisablePOI();
		for (AActor* Gate : Gates)
		{
			IAuraLockedInterface::Unlock(Gate);
		}
	}
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
	if (const UAuraInventoryComponent* InventoryComponent = UAuraInventoryComponent::Get(Player))
	{
		return InventoryComponent->HasItemInInventory(KeyTag);
	}
	return false;
}

void AAuraPOILock::HandleInteract_Implementation(AActor* Player)
{
	Unlock(Player);
}

void AAuraPOILock::Unlock(AActor* Player)
{
	if (bUnlocked)
	{
		return;
	}
	if (UAuraInventoryComponent* InventoryComponent = UAuraInventoryComponent::Get(Player))
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
		}
	}
}
