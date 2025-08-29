// Copyright Alien Shores


#include "Game/AuraGameInstance.h"

#include "Kismet/GameplayStatics.h"

UAuraGameInstance* UAuraGameInstance::Get(const UObject* WorldContextObject)
{
	if (IsValid(WorldContextObject))
	{
		return Cast<UAuraGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	}
	return nullptr;
}

void UAuraGameInstance::Shutdown()
{
	Super::Shutdown();
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
