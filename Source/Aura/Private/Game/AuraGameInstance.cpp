// Copyright Alien Shores


#include "Game/AuraGameInstance.h"

#include "Aura/AuraLogChannels.h"
#include "Kismet/GameplayStatics.h"

void UAuraGameInstance::Shutdown()
{
	Super::Shutdown();
	if (bTransient && bAutoCleanup)
	{
		UE_LOG(LogAura, Warning, TEXT("[%s] Deleting save game: %s"), *GetName(), *LoadSlotName);
		UGameplayStatics::DeleteGameInSlot(LoadSlotName, LoadSlotIndex);
	}
}
