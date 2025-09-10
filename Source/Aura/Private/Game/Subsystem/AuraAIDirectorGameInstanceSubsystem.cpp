// Copyright Alien Shores


#include "Game/Subsystem/AuraAIDirectorGameInstanceSubsystem.h"

#include "Aura/AuraLogChannels.h"
#include "Kismet/GameplayStatics.h"

UAuraAIDirectorGameInstanceSubsystem* UAuraAIDirectorGameInstanceSubsystem::Get(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject))
	{
		UE_LOG(LogAura, Warning, TEXT("[%s] Invalid world context object provided - cannot lookup game instance subsystem"), *FString("UAuraAIDirectorGameInstanceSubsystem::Get"));
		return nullptr;
	}
	if (const UGameInstance* GameInstance = IsValid(WorldContextObject) ? UGameplayStatics::GetGameInstance(WorldContextObject) : nullptr)
	{
		return GameInstance->GetSubsystem<UAuraAIDirectorGameInstanceSubsystem>();
	}
	UE_LOG(LogAura, Warning, TEXT("[%s] No subsystem found for context object: %s"), *FString("UAuraAIDirectorGameInstanceSubsystem::Get"), *WorldContextObject->GetName());
	return nullptr;
}

TArray<AActor*> UAuraAIDirectorGameInstanceSubsystem::GetActivePlayerActors()
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
