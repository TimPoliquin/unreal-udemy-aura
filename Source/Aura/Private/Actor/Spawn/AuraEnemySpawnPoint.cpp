// Copyright Alien Shores


#include "Actor/Spawn/AuraEnemySpawnPoint.h"

#include "Aura/AuraLogChannels.h"

AAuraEnemySpawnPoint::AAuraEnemySpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;
}

AAuraEnemy* AAuraEnemySpawnPoint::SpawnEnemy() const
{
	AAuraEnemy* Enemy = SpawnConfig.Spawn(this);
	if (!Enemy)
	{
		UE_LOG(LogAura, Warning, TEXT("[%s] Failed to spawn enemy - check configuration!"), *GetName());
	}
	return Enemy;
}
