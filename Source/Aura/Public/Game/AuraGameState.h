// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Interaction/SavableInterface.h"
#include "AuraGameState.generated.h"

class UPlayerInventoryComponent;
/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class AURA_API AAuraGameState : public AGameStateBase, public ISavableInterface
{
	GENERATED_BODY()

public:
	static AAuraGameState* Get(const UObject* WorldContextObject);

	AAuraGameState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UPlayerInventoryComponent* GetPlayerInventoryComponent() const;

	/** Start ISavableInterface **/
	virtual void FromSaveData(const UAuraSaveGame* SaveData) override;
	virtual void ToSaveData(UAuraSaveGame* SaveData) const override;
	/** End ISavableInterface **/

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Replicated)
	TObjectPtr<UPlayerInventoryComponent> PlayerInventoryComponent;
};
