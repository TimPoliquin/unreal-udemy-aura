// Copyright Alien Shores


#include "Actor/AuraEnemySpawnVolume.h"

#include "Actor/AuraEnemySpawnPoint.h"
#include "Components/BoxComponent.h"
#include "Interaction/PlayerInterface.h"


// Sets default values
AAuraEnemySpawnVolume::AAuraEnemySpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	SetRootComponent(Box);
	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Box->SetCollisionResponseToAllChannels(ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AAuraEnemySpawnVolume::LoadActor_Implementation()
{
	if (HasActorBegunPlay() && bTriggered)
	{
		SetEnabled(false);
	}
}

void AAuraEnemySpawnVolume::SetEnabled(const bool bInEnabled)
{
	bEnabled = bInEnabled;
	SetActorHiddenInGame(!bInEnabled);
	SetActorEnableCollision(bInEnabled);
	if (!bInEnabled)
	{
		SetActorTickEnabled(false);
	}
}

void AAuraEnemySpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	if (!bEnabled)
	{
		SetEnabled(false);
	}
	if (bTriggered)
	{
		SetEnabled(false);
		return;
	}
	Box->OnComponentBeginOverlap.AddDynamic(this, &AAuraEnemySpawnVolume::OnBoxOverlap);
}

void AAuraEnemySpawnVolume::OnBoxOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!IPlayerInterface::ImplementsPlayerInterface(OtherActor) || !bEnabled)
	{
		return;
	}
	bTriggered = true;
	for (AAuraEnemySpawnPoint* SpawnPoint : SpawnPoints)
	{
		if (IsValid(SpawnPoint))
		{
			SpawnPoint->SpawnEnemy();
		}
	}
	SetEnabled(false);
}
