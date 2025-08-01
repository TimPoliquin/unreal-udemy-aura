// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraSpawnEventPayloadDefinitions.h"
#include "GameFramework/Actor.h"
#include "AuraSpawnTunnel.generated.h"

struct FEnemySpawnConfig;
class AAuraSpawnTunnel;
class UAuraActorTrackerComponent;
class AAuraEnemy;

USTRUCT(BlueprintType)
struct FOnAuraSpawnTunnelDefeatedPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AAuraSpawnTunnel> SpawnTunnel;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAuraSpawnTunnelDefeatedSignature, const FOnAuraSpawnTunnelDefeatedPayload&, Payload);

UCLASS()
class AURA_API AAuraSpawnTunnel : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAuraSpawnTunnel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintAssignable)
	FOnAuraSpawnTunnelDefeatedSignature OnAuraSpawnTunnelDefeated;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAuraActorTrackerComponent> EnemyTrackerComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UChildActorComponent> SpawnPointComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	TArray<FEnemySpawnConfig> EnemyConfig;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	float MinSpawnDelay = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	float MaxSpawnDelay = 1.f;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Spawn")
	void ActivateSpawnTunnel(const float NumEnemies);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetRandomSpawnDelay() const;
	UFUNCTION(BlueprintCallable)
	TArray<AAuraEnemy*> InitializeEnemies(const float NumToSpawn);
	UFUNCTION(BlueprintCallable)
	void FinishSpawningEnemy(AAuraEnemy* Enemy);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void DeactivateSpawnTunnel();

private:
	UFUNCTION()
	void OnEnemyCountChanged(const FOnAuraActorTrackerCountChangedPayload& Payload);
};
