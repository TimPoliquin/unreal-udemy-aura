// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/AuraAbilitySystemInterface.h"
#include "Game/Save/SaveableInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class UAuraAttributeSet;
class UAuraAbilitySystemComponent;
class UAuraInventoryComponent;
class ULevelUpInfo;

class UAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface, public IAuraAbilitySystemInterface, public ISaveableInterface
{
	GENERATED_BODY()

public:
	AAuraPlayerState();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UAuraAttributeSet* GetAuraAttributeSet() const;

	/** Start IAuraAbilitySystemInterface **/
	virtual int32 GetCharacterLevel_Implementation() const override;
	/** End IAuraAbilitySystemInterface **/

	/** Start ISaveableInterface **/
	virtual TArray<uint8> SaveData_Implementation() override;
	virtual bool LoadData_Implementation(const TArray<uint8>& InData) override;
	virtual bool ShouldSave_Implementation() const override { return false; }
	/** End ISaveableInterface **/

	void InitializeAbilityActorInfo();

protected:
	virtual void BeginPlay() override;
	UPROPERTY(Replicated)
	TObjectPtr<UAuraAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	TObjectPtr<UAuraAttributeSet> AttributeSet;
};
