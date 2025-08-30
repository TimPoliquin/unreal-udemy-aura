// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	static AAuraGameModeBase* GetAuraGameMode(const UObject* WorldContextObject);
};
