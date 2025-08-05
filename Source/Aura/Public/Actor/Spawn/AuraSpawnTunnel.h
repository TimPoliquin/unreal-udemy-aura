// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraSpawnEventPayloadDefinitions.h"
#include "AuraSpawnInterface.h"
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
class AURA_API AAuraSpawnTunnel : public AActor, public IAuraSpawnInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAuraSpawnTunnel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Start AuraSpawnInterface **/
	virtual FOnAuraActorTrackerCountChangedDelegate& GetOnCountChangedDelegate() override;
	virtual void BeginSpawning_Implementation(const int32 NumEnemies, const FRandRange& SpawnDelay, const TArray<FEnemySpawnConfig>& SpawnClasses) override;

	/** End AuraSpawnInterface **/

	UPROPERTY(BlueprintAssignable)
	FOnAuraSpawnTunnelDefeatedSignature OnAuraSpawnTunnelDefeated;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAuraActorTrackerComponent> EnemyTrackerComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UChildActorComponent> SpawnPointComponent;

	UFUNCTION(BlueprintCallable)
	TArray<AAuraEnemy*> InitializeEnemies(int32 NumToSpawn, const TArray<FEnemySpawnConfig>& SpawnClasses);
	UFUNCTION(BlueprintCallable)
	void FinishSpawningEnemy(AAuraEnemy* Enemy);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void DeactivateSpawnTunnel();

private:
	UFUNCTION()
	void OnEnemyCountChanged(const FOnAuraActorTrackerCountChangedPayload& Payload);
};
