// Copyright Alien Shores


#include "Actor/Spawn/AuraEnemySpawnPoint.h"

AAuraEnemySpawnPoint::AAuraEnemySpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;
}

AAuraEnemy* AAuraEnemySpawnPoint::SpawnEnemy() const
{
	return SpawnConfig.Spawn(this);
}
