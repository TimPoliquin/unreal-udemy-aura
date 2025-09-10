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
