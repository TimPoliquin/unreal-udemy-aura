// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "AuraEffectActor.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerOverlapSignature, AActor*, Player);

UENUM(BlueprintType)
enum class EEffectApplicationPolicy : uint8
{
	ApplyOnOverlap,
	ApplyOnEndOverlap,
	DoNotApply
};

UENUM(BlueprintType)
enum class EEffectRemovalPolicy : uint8
{
	RemoveOnEndOverlap,
	DoNotRemove
};

USTRUCT(BlueprintType)
struct FGameplayEffectConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> GameplayEffectClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EEffectApplicationPolicy EffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;
	/** RemovalPolicy is only applicable to Infinite effects **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	EEffectRemovalPolicy RemovalPolicy = EEffectRemovalPolicy::DoNotRemove;

	bool IsApplyOnOverlap() const { return EffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap; }
	bool IsApplyOnEndOverlap() const { return EffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap; }
	bool IsRemoveOnEndOverlap() const { return RemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap; }
};

UCLASS()
class AURA_API AAuraEffectActor : public AActor
{
	GENERATED_BODY()

public:
	AAuraEffectActor();
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerOverlapSignature OnPlayerOverlapStart;
	UPROPERTY(BlueprintAssignable)
	FOnPlayerOverlapSignature OnPlayerOverlapEnd;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor, const FGameplayEffectConfig& GameplayEffectConfig);
	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);
	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	TArray<FGameplayEffectConfig> GameplayEffectConfigs;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	bool bDestroyOnEffectApplication = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	TArray<FName> ApplyToTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Applied Effects")
	float ActorLevel = 1.f;

	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement")
	bool bRotates = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement")
	float RotationRate = 45.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement")
	bool bSinusoidalMovement = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement")
	float SineAmplitude = 8.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement")
	float SinePeriodMultiplier = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement")
	FVector InitialLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Movement")
	bool bPlaySpawnEffect = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pickup Sound")
	TObjectPtr<USoundBase> SpawnSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pickup Sound")
	TObjectPtr<USoundBase> PickupSound;

	UFUNCTION(BlueprintCallable)
	void StartSinusoidalMovement();
	UFUNCTION(BlueprintCallable)
	void StartRotation();
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void PlaySpawnEffect();

private:
	void RemoveEffectsFromTarget(AActor* TargetActor);
	void Bob(const float DeltaTime);
	float RunningTime = 0.f;
};
