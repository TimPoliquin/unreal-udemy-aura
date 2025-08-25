// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AuraGameInstance.generated.h"

UENUM()
enum class EAuraGameSaveState : uint8
{
	Undefined,
	Transient,
	SaveSlot
};

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FName PlayerStartTag = FName();

	UPROPERTY()
	FString LoadSlotName;

	UPROPERTY()
	int32 LoadSlotIndex;

	UPROPERTY()
	EAuraGameSaveState SaveState = EAuraGameSaveState::Undefined;

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> ActivePlayerActors;

	virtual void Shutdown() override;

	EAuraGameSaveState GetSaveState() const { return SaveState; }

	void RegisterActivePlayer(AActor* Actor)
	{
		ActivePlayerActors.Add(Actor);
	}

	void UnregisterActivePlayer(AActor* Actor)
	{
		ActivePlayerActors.Remove(Actor);
	}

	TArray<AActor*> GetActivePlayerActors();
};
