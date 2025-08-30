// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AuraAIDirectorGameInstanceSubsystem.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class AURA_API UAuraAIDirectorGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UAuraAIDirectorGameInstanceSubsystem* Get(const UObject* WorldContextObject);

	void RegisterActivePlayer(AActor* Actor)
	{
		ActivePlayerActors.Add(Actor);
	}

	void UnregisterActivePlayer(AActor* Actor)
	{
		ActivePlayerActors.Remove(Actor);
	}

	TArray<AActor*> GetActivePlayerActors();

private:
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> ActivePlayerActors;
};
