// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
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
	TSubclassOf<AAuraEnemy> EnemyClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemey Class")
	int32 EnemyLevel = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemey Class")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	UFUNCTION(BlueprintCallable, Category = "Enemey Class")
	void SpawnEnemy();
};
