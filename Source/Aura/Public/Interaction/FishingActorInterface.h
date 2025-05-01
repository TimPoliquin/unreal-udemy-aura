// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FishingActorInterface.generated.h"

class UNiagaraSystem;
class IFishingComponentInterface;
// This class does not need to be modified.
UINTERFACE()
class UFishingActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IFishingActorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TScriptInterface<IFishingComponentInterface> GetFishingComponent() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ShowFishingStatusEffect(UNiagaraSystem* EffectSystem);

	static TScriptInterface<IFishingComponentInterface> GetFishingComponent(const UObject* Actor);
};
