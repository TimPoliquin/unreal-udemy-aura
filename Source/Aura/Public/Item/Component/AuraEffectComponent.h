// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Item/AuraEffectTypes.h"
#include "Components/ActorComponent.h"
#include "AuraEffectComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AURA_API UAuraEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAuraEffectComponent();

	UPROPERTY(BlueprintAssignable)
	FOnPlayerOverlapSignature OnPlayerOverlapStart;
	UPROPERTY(BlueprintAssignable)
	FOnPlayerOverlapSignature OnPlayerOverlapEnd;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool CheckPreRequisites(AActor* PickupActor) const;
	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);
	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);
	void SetDefaults(
		const TArray<FGameplayEffectConfig>& InEffectConfig,
		const bool InDestroyOnEffectApplication,
		const TArray<FName>& InApplyToTags
	);

protected:
	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor, const FGameplayEffectConfig& GameplayEffectConfig);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	TArray<FGameplayEffectConfig> GameplayEffectConfigs;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	bool bDestroyOnEffectApplication = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	TArray<FName> ApplyToTags;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Applied Effects")
	float ActorLevel = 1.f;

private:
	UPROPERTY()
	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles;
	void RemoveEffectsFromTarget(AActor* TargetActor);
};
