// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraSpawnEventPayloadDefinitions.h"
#include "GameFramework/Actor.h"
#include "Interaction/SaveInterface.h"
#include "AuraEnemySpawnVolume.generated.h"

class UAuraActorTrackerComponent;
class AAuraEnemySpawnPoint;
class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAuraEnemySpawnVolumeAllEnemiesDefeatedSignature);

UCLASS()
class AURA_API AAuraEnemySpawnVolume : public AActor, public ISaveInterface
{
	GENERATED_BODY()

public:
	AAuraEnemySpawnVolume();
	void SetEnabled(const bool bInEnabled);

	/** Save Interface **/
	virtual void LoadActor_Implementation() override;
	/** Save Interface End**/

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAuraEnemySpawnVolumeAllEnemiesDefeatedSignature OnAllEnemiesDefeated;

protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnBoxOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="Properties|Debug")
	bool bEnabled = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category="Properties|Debug")
	bool bTriggered = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UAuraActorTrackerComponent> ActorTrackerComponent;

private:
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UBoxComponent> Box;
	UPROPERTY(EditAnywhere, Category="Properties")
	TArray<AAuraEnemySpawnPoint*> SpawnPoints;

	UFUNCTION()
	void OnActorTrackerCountChangedHandler(const FOnAuraActorTrackerCountChangedPayload& Payload);
};
