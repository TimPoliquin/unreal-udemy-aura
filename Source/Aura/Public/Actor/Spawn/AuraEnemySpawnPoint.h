// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraEnemySpawnConfig.h"
#include "Engine/TargetPoint.h"
#include "AuraEnemySpawnPoint.generated.h"

class AAuraEnemy;

UCLASS()
class AURA_API AAuraEnemySpawnPoint : public ATargetPoint
{
	GENERATED_BODY()

public:
	AAuraEnemySpawnPoint();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemey Class")
	FEnemySpawnConfig SpawnConfig;

	UFUNCTION(BlueprintCallable, Category = "Enemey Class")
	AAuraEnemy* SpawnEnemy() const;
};
