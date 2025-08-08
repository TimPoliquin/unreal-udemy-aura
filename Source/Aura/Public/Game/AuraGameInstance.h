// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AuraGameInstance.generated.h"

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
	FString LoadSlotName = FString("Transient");

	UPROPERTY()
	int32 LoadSlotIndex = 9;

	UPROPERTY()
	bool bTransient = true;

	UPROPERTY()
	bool bAutoCleanup = false;

	virtual void Shutdown() override;
};
