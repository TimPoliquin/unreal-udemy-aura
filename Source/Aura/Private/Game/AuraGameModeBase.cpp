// Copyright Alien Shores


#include "Game/AuraGameModeBase.h"

#include "Aura/AuraLogChannels.h"
#include "Game/AuraGameInstance.h"
#include "Game/AuraSaveGame.h"
#include "Game/Subsystem/LevelGameInstanceSubsystem.h"
#include "Game/Subsystem/LocalPlayerSaveGameSubsystem.h"
#include "GameFramework/PlayerStart.h"
#include "Item/Data/AuraItemInfo.h"
#include "Kismet/GameplayStatics.h"


AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	if (const APlayerController* PlayerController = Cast<APlayerController>(Player))
	{
		const ULevelGameInstanceSubsystem* LevelSubsystem = ULevelGameInstanceSubsystem::Get(PlayerController);
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

UAuraGameInstance* AAuraGameModeBase::GetAuraGameInstance() const
{
	return Cast<UAuraGameInstance>(GetGameInstance());
}

FAuraItemDefinition AAuraGameModeBase::FindItemDefinitionByItemTag(const FGameplayTag& ItemTag)
{
	if (ItemDefinitions.IsEmpty())
	{
		InitializeItemDefinitions();
	}
	if (ItemDefinitions.Contains(ItemTag))
	{
		return ItemDefinitions[ItemTag];
	}
	return FAuraItemDefinition();
}

FGameplayTag AAuraGameModeBase::GetDefaultItemPickupMessageTag() const
{
	return DefaultItemPickupMessageTag;
}

FGameplayTag AAuraGameModeBase::GetDefaultItemUseMessageTag() const
{
	return DefaultItemUsedMessageTag;
}

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	InitializeItemDefinitions();
}

AAuraGameModeBase* AAuraGameModeBase::GetAuraGameMode(const UObject* WorldContextObject)
{
	return Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
}

void AAuraGameModeBase::InitializeItemDefinitions()
{
	if (!ItemDefinitions.IsEmpty())
	{
		return;
	}
	for (const UAuraItemInfo* ItemDefinitionSet : ItemInfos)
	{
		ItemDefinitionSet->AddToMap(ItemDefinitions);
	}
}
