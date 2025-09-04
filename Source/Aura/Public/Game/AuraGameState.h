// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "AuraGameState.generated.h"

class UAuraProgressionComponent;
class UAuraInventoryComponent;
/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class AURA_API AAuraGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	static AAuraGameState* Get(const UObject* WorldContextObject);

	AAuraGameState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UAuraInventoryComponent* GetInventoryComponent() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UAuraProgressionComponent* GetProgressionComponent() const;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Replicated, Category="Components")
	TObjectPtr<UAuraInventoryComponent> InventoryComponent;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Replicated, Category="Components")
	TObjectPtr<UAuraProgressionComponent> ProgressionComponent;
};
