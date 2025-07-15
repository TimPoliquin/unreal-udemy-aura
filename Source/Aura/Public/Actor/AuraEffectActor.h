// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "AuraEffectActor.generated.h"

class UAuraEffectComponent;
class UAbilitySystemComponent;
class UGameplayEffect;

UCLASS()
class AURA_API AAuraEffectActor : public AActor
{
	GENERATED_BODY()

public:
	AAuraEffectActor();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAuraEffectComponent> EffectComponent;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	bool bPlaySpawnEffect = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawn")
	TObjectPtr<USoundBase> SpawnSound;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void PlaySpawnEffect();

private:
	UPROPERTY()
	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles;
};
