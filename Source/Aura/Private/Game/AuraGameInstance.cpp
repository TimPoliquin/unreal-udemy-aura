// Copyright Alien Shores


#include "Game/AuraGameInstance.h"

#include "Aura/AuraLogChannels.h"
#include "Kismet/GameplayStatics.h"

void UAuraGameInstance::Shutdown()
{
	Super::Shutdown();
	if (SaveState == EAuraGameSaveState::Undefined || SaveState == EAuraGameSaveState::Transient)
	{
		UE_LOG(LogAura, Warning, TEXT("[%s] Deleting save game: %s"), *GetName(), *LoadSlotName);
		UGameplayStatics::DeleteGameInSlot(LoadSlotName, LoadSlotIndex);
	}
}

TArray<AActor*> UAuraGameInstance::GetActivePlayerActors()
{
	TArray<AActor*> Results;
	for (TWeakObjectPtr<AActor> Actor : ActivePlayerActors)
	{
		if (Actor.IsValid())
		{
			Results.Add(Actor.Get());
		}
	}
	return Results;
}
