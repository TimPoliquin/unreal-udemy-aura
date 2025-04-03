// Copyright Alien Shores


#include "Checkpoint/MapEntrance.h"

#include "Game/AuraGameModeBase.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"


AMapEntrance::AMapEntrance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMapEntrance::LoadActor_Implementation()
{
	// Do nothing
}

void AMapEntrance::BeginPlay()
{
	Super::BeginPlay();
}

void AMapEntrance::OnSphereOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (IsValid(OtherActor) && OtherActor->Implements<UPlayerInterface>())
	{
		bHasBeenActivated = true;
		IPlayerInterface::Execute_SaveProgress(OtherActor, PlayerStartTag);
		AAuraGameModeBase* GameMode = AAuraGameModeBase::GetAuraGameMode(this);
		GameMode->SaveWorldState(GetWorld(), DestinationMap.ToSoftObjectPath().GetAssetName());
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, DestinationMap);
	}
}
