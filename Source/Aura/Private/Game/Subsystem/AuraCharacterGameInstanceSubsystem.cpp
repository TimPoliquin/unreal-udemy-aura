// Copyright Alien Shores


#include "AuraCharacterGameInstanceSubsystem.h"

#include "Kismet/GameplayStatics.h"

UAuraCharacterGameInstanceSubsystem* UAuraCharacterGameInstanceSubsystem::Get(const UObject* WorldContextObject)
{
	if (const UGameInstance* GameInstance = IsValid(WorldContextObject)
		                                        ? UGameplayStatics::GetGameInstance(WorldContextObject)
		                                        : nullptr)
	{
		return GameInstance->GetSubsystem<UAuraCharacterGameInstanceSubsystem>();
	}
	return nullptr;
}
