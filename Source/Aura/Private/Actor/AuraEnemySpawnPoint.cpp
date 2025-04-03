// Copyright Alien Shores


#include "Actor/AuraEnemySpawnPoint.h"

#include "Character/AuraEnemy.h"


AAuraEnemySpawnPoint::AAuraEnemySpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AAuraEnemySpawnPoint::SpawnEnemy()
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AAuraEnemy* Enemy = GetWorld()->SpawnActorDeferred<AAuraEnemy>(EnemyClass, GetActorTransform());
	Enemy->SetLevel(EnemyLevel);
	Enemy->SetCharacterClass(CharacterClass);
	Enemy->FinishSpawning(GetActorTransform());
	Enemy->SpawnDefaultController();
}
