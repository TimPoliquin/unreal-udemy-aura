// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/AttributeChangeDelegates.h"
#include "Game/Save/SaveableInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class UAuraAttributeSet;
class UAuraAbilitySystemComponent;
class UAuraInventoryComponent;
class UOLD_AuraSaveGame;
class ULevelUpInfo;


class UAttributeSet;
class UAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface, public ISaveableInterface
{
	GENERATED_BODY()

public:
	AAuraPlayerState();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UAttributeSet* GetAttributeSet() const;

	/** Start ISaveableInterface **/
	/** End ISaveableInterface **/
	void InitializeAbilityActorInfo();

protected:
	UPROPERTY(Replicated)
	TObjectPtr<UAuraAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(Replicated)
	TObjectPtr<UAuraAttributeSet> AttributeSet;
};
