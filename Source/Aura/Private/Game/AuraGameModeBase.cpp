// Copyright Alien Shores


#include "Game/AuraGameModeBase.h"

#include "Aura/AuraLogChannels.h"
#include "Game/Subsystem/AuraLevelManager.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"


AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	if (const APlayerController* PlayerController = Cast<APlayerController>(Player))
	{
		const UAuraLevelManager* LevelSubsystem = UAuraLevelManager::Get(PlayerController);
		const FName PlayerStartTag = LevelSubsystem
			                             ? LevelSubsystem->GetCurrentPlayerStartTag(PlayerController, true)
			                             : FName();
		UE_LOG(LogAura, Log, TEXT("[%s] Finding player start with tag: %s"), *GetName(), *PlayerStartTag.ToString());
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Actors);
		if (Actors.Num() > 0)
		{
			AActor* SelectedActor = Actors[0];
			for (AActor* Actor : Actors)
			{
				if (APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
				{
					if (PlayerStart->PlayerStartTag == PlayerStartTag)
					{
						SelectedActor = PlayerStart;
						break;
					}
				}
			}
			return SelectedActor;
		}
	}
	return nullptr;
}

AAuraGameModeBase* AAuraGameModeBase::GetAuraGameMode(const UObject* WorldContextObject)
{
	return Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
}
