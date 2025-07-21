#include "Actor/Spawn/AuraEnemySpawnConfig.h"

#include "Character/AuraEnemy.h"

AAuraEnemy* FEnemySpawnConfig::Spawn(const AActor* SpawnPoint) const
{
	if (AAuraEnemy* Enemy = SpawnDeferred(SpawnPoint))
	{
		Enemy->FinishSpawning(SpawnPoint->GetActorTransform());
		Enemy->SpawnDefaultController();
		return Enemy;
	}
	return nullptr;
}

AAuraEnemy* FEnemySpawnConfig::SpawnDeferred(const AActor* SpawnPoint) const
{
	if (!IsValid(SpawnPoint))
	{
		return nullptr;
	}
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AAuraEnemy* Enemy = SpawnPoint->GetWorld()->SpawnActorDeferred<AAuraEnemy>(EnemyClass, SpawnPoint->GetActorTransform());
	Enemy->SetLevel(EnemyLevel);
	Enemy->SetCharacterClass(CharacterClass);
	return Enemy;
}
