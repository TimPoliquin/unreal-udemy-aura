// Copyright Alien Shores


#include "Actor/Hoard/AuraHoardModeComponent.h"

#include "Actor/Hoard/AuraHoardModeConfig.h"
#include "Actor/Spawn/AuraSpawnInterface.h"
#include "Actor/Spawn/AuraSpawnEventPayloadDefinitions.h"
#include "Aura/AuraLogChannels.h"
#include "Utils/ArrayUtils.h"


// Sets default values for this component's properties
UAuraHoardModeComponent::UAuraHoardModeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UAuraHoardModeComponent::HasNextRound() const
{
	return HoardModeConfig->HasConfigurationForRound(CurrentRound + 1);
}


void UAuraHoardModeComponent::PrepareNextRound()
{
	CurrentRound++;
}


void UAuraHoardModeComponent::StartRound()
{
	if (!HoardModeConfig)
	{
		UE_LOG(LogAura, Warning, TEXT("[%s.%s] No hoard mode configuration set!"), *GetOwner()->GetName(), *GetName());
		return;
	}
	if (!HoardModeConfig->HasConfigurationForRound(CurrentRound))
	{
		UE_LOG(LogAura, Warning, TEXT("[%s.%s] No hoard mode configuration for round %d"), *GetOwner()->GetName(), *GetName(), CurrentRound);
		UE_LOG(LogAura, Warning, TEXT("[%s.%s] No hoard mode configuration for round %d"), *GetOwner()->GetName(), *GetName(), CurrentRound);
		return;
	}
	const FHoardRoundConfig& RoundConfig = HoardModeConfig->GetRoundConfig(CurrentRound);
	TArray<AActor*> AvailableSpawnPoints = SpawnPoints;
	UArrayUtils::ShuffleArray(AvailableSpawnPoints);
	for (int32 SpawnPointIdx = 0; SpawnPointIdx < RoundConfig.NumSpawnPoints; SpawnPointIdx++)
	{
		if (!AvailableSpawnPoints.IsValidIndex(SpawnPointIdx))
		{
			UE_LOG(LogAura, Warning,
			       TEXT("[%s.%s] Invalid spawn point index - not enough spawn points configured! Round: %d, NumSpawnPoints: %d"),
			       *GetOwner()->GetName(),
			       *GetName(),
			       CurrentRound,
			       SpawnPointIdx
			);
			break;
		}
		AActor* SpawnPointActor = AvailableSpawnPoints[SpawnPointIdx];
		if (IAuraSpawnInterface* SpawnPointInterface = Cast<IAuraSpawnInterface>(SpawnPointActor))
		{
			SpawnPointInterface->GetOnCountChangedDelegate().AddDynamic(this, &UAuraHoardModeComponent::OnSpawnPointCountChanged);
			ActiveSpawnPoints.Add(SpawnPointActor);
			IAuraSpawnInterface::Execute_BeginSpawning(SpawnPointActor, RoundConfig.NumEnemiesToSpawn, RoundConfig.SpawnDelay, RoundConfig.EnemyConfig);
		}
	}
}

void UAuraHoardModeComponent::EndRound()
{
	ActiveSpawnPoints.Empty();
	const int32 CompletedRound = CurrentRound;
	TArray<FHoardRoundRewards> Rewards;
	if (HasNextRound())
	{
		PrepareNextRound();
		const int32 NextRound = CurrentRound;
		OnHoardModeRoundComplete.Broadcast(FOnHoardModeRoundCompletePayload(CompletedRound, NextRound, true, HoardModeConfig->GetRewards(CompletedRound)));
	}
	else
	{
		CurrentRound = 0;
		OnHoardModeRoundComplete.Broadcast(FOnHoardModeRoundCompletePayload(CompletedRound, -1, false, HoardModeConfig->GetRewards(CompletedRound)));
	}
}


void UAuraHoardModeComponent::OnSpawnPointCountChanged(const FOnAuraActorTrackerCountChangedPayload& Payload)
{
	if (Payload.IsEmpty() && Payload.Owner)
	{
		if (IAuraSpawnInterface* SpawnInterface = Cast<IAuraSpawnInterface>(Payload.Owner))
		{
			SpawnInterface->GetOnCountChangedDelegate().RemoveDynamic(this, &UAuraHoardModeComponent::OnSpawnPointCountChanged);
		}
		ActiveSpawnPoints.RemoveAll([Payload](const AActor* SpawnPoint)
		{
			return SpawnPoint == Payload.Owner || !IsValid(SpawnPoint);
		});
		if (ActiveSpawnPoints.Num() <= 0)
		{
			EndRound();
		}
	}
}
