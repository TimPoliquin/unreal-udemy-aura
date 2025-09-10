// Copyright Alien Shores


#include "Checkpoint/MapEntrance.h"

#include "Game/Save/AuraSaveGameManager.h"
#include "Game/Subsystem/AuraLevelManager.h"
#include "Game/Subsystem/Old_SaveGameManager.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"


AMapEntrance::AMapEntrance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	bDisableAfterActivation = false;
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
		if (UAuraLevelManager* LevelManager = UAuraLevelManager::Get(this))
		{
			FAuraLevelTransitionParams Params;
			Params.SaveSlot = UAuraSaveGameManager::Get(this)->GetAutoSaveSlotName();
			Params.bShouldSave = true;
			Params.bShouldLoad = true;
			Params.MapAssetName = DestinationMap.GetAssetName();
			Params.PlayerStartTag = DestinationPlayerStartTag;
			LevelManager->TransitionLevel(Params);
		}
	}
}
