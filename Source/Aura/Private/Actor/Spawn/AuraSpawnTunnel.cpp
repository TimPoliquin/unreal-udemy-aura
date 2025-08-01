// Copyright Alien Shores


#include "Actor/Spawn/AuraSpawnTunnel.h"

#include "Actor/Spawn/AuraActorTrackerComponent.h"
#include "Actor/Spawn/AuraEnemySpawnConfig.h"
#include "Aura/AuraLogChannels.h"
#include "Character/AuraEnemy.h"
#include "Engine/TargetPoint.h"
#include "Utils/ArrayUtils.h"


// Sets default values
AAuraSpawnTunnel::AAuraSpawnTunnel()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	SetRootComponent(StaticMeshComponent);
	SpawnPointComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("SpawnPoint Component"));
	SpawnPointComponent->SetChildActorClass(ATargetPoint::StaticClass());
	SpawnPointComponent->SetupAttachment(GetRootComponent());
	EnemyTrackerComponent = CreateDefaultSubobject<UAuraActorTrackerComponent>(TEXT("Enemy Tracker Component"));
}

// Called when the game starts or when spawned
void AAuraSpawnTunnel::BeginPlay()
{
	Super::BeginPlay();
	EnemyTrackerComponent->OnCountChanged.AddDynamic(this, &AAuraSpawnTunnel::OnEnemyCountChanged);
}


float AAuraSpawnTunnel::GetRandomSpawnDelay() const
{
	return FMath::RandRange(MinSpawnDelay, MaxSpawnDelay);
}

TArray<AAuraEnemy*> AAuraSpawnTunnel::InitializeEnemies(const float NumToSpawn)
{
	TArray<AAuraEnemy*> Enemies;
	for (int32 Idx = 0; Idx < NumToSpawn; Idx++)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		const FEnemySpawnConfig& SpawnConfig = UArrayUtils::GetRandomElement(EnemyConfig);
		AAuraEnemy* Enemy = SpawnPointComponent->GetWorld()->SpawnActorDeferred<AAuraEnemy>(SpawnConfig.EnemyClass, SpawnPointComponent->GetChildActor()->GetActorTransform());
		Enemy->SetLevel(SpawnConfig.EnemyLevel);
		Enemies.Add(Enemy);
	}
	return Enemies;
}

void AAuraSpawnTunnel::FinishSpawningEnemy(AAuraEnemy* Enemy)
{
	EnemyTrackerComponent->Track(Enemy);
	Enemy->FinishSpawning(SpawnPointComponent->GetChildActor()->GetActorTransform());
	Enemy->SpawnDefaultController();
}

void AAuraSpawnTunnel::OnEnemyCountChanged(const FOnAuraActorTrackerCountChangedPayload& Payload)
{
	if (Payload.IsEmpty())
	{
		DeactivateSpawnTunnel();
		OnAuraSpawnTunnelDefeated.Broadcast(FOnAuraSpawnTunnelDefeatedPayload(this));
	}
}
