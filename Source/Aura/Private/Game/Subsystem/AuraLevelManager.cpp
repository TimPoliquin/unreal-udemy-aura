// Copyright Alien Shores


#include "Game/Subsystem/AuraLevelManager.h"

#include "Aura/AuraLogChannels.h"
#include "Game/Save/AuraSaveGameManager.h"
#include "Game/Subsystem/AuraLevelTransition.h"
#include "Kismet/GameplayStatics.h"

void UAuraLevelManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogAura, Warning, TEXT("[%s] Initialize"), *GetName())
}

void UAuraLevelManager::Deinitialize()
{
	Super::Deinitialize();
}

TSoftObjectPtr<UWorld> UAuraLevelManager::GetMapFromMapDisplayName(const FString& MapDisplayName) const
{
	return GetMapConfigByDisplayName(MapDisplayName).Map;
}

FString UAuraLevelManager::GetMapNameFromMapAssetName(const FString& MapAssetName) const
{
	return GetMapConfigByMapAssetName(MapAssetName).MapDisplayName;
}

UAuraLevelManager* UAuraLevelManager::Get(const UObject* WorldContextObject)
{
	if (const UGameInstance* GameInstance = IsValid(WorldContextObject)
		                                        ? UGameplayStatics::GetGameInstance(WorldContextObject)
		                                        : nullptr)
	{
		return GameInstance->GetSubsystem<UAuraLevelManager>();
	}
	return nullptr;
}

void UAuraLevelManager::LoadMap(const UObject* WorldContextObject, const FString& MapAssetName) const
{
	const FAuraMapConfig& SelectedMapConfig = GetMapConfigByMapAssetName(MapAssetName);
	if (SelectedMapConfig.IsValid() && !SelectedMapConfig.Map.IsNull())
	{
		UGameplayStatics::OpenLevelBySoftObjectPtr(WorldContextObject, SelectedMapConfig.Map);
	}
	else
	{
		UE_LOG(LogAura, Error, TEXT("[%s] Failed to load map by display name: %s"), *GetName(), *MapAssetName);
		for (const FAuraMapConfig& MapConfig : MapConfigs)
		{
			UE_LOG(
				LogAura,
				Error,
				TEXT("[%s] Map: %s - %s"),
				*GetName(),
				*MapConfig.MapDisplayName,
				*MapConfig.Map.ToSoftObjectPath().GetAssetName()
			);
		}
	}
}


FString UAuraLevelManager::GetDefaultMapName() const
{
	return DefaultMapName;
}

FString UAuraLevelManager::GetDefaultMapAssetName() const
{
	return GetMapFromMapDisplayName(DefaultMapName).ToSoftObjectPath().GetAssetName();
}

FName UAuraLevelManager::GetDefaultPlayerStartTag(const FString& MapAssetName) const
{
	return GetMapConfigByMapAssetName(MapAssetName).DefaultPlayerStartTag;
}

int32 UAuraLevelManager::GetDefaultPlayerLevel(const FString& MapAssetName) const
{
	return GetMapConfigByMapAssetName(MapAssetName).DefaultPlayerLevel;
}

FName UAuraLevelManager::GetCurrentPlayerStartTag(
	const UObject* WorldContextObject,
	const bool bFallbackToDefault
) const
{
	if (LevelTransition)
	{
		return LevelTransition->GetDestinationPlayerStartTag();
	}
	if (bFallbackToDefault)
	{
		const UWorld* World = WorldContextObject->GetWorld();
		FString MapName = World->GetMapName();
		MapName.RemoveFromStart(World->StreamingLevelsPrefix);
		const FAuraMapConfig& MapConfig = GetMapConfigByMapAssetName(MapName);
		return MapConfig.DefaultPlayerStartTag;
	}
	return NAME_None;
}

void UAuraLevelManager::TransitionLevel(const FAuraLevelTransitionParams& Params)
{
	if (Params.ShouldSave())
	{
		UAuraSaveGameManager* SaveGameManager = UAuraSaveGameManager::Get(GetWorld());
		FAuraSaveGameParams SaveGameParams;
		SaveGameParams.DestinationMapName = Params.MapAssetName;
		SaveGameParams.DestinationPlayerStartTag = Params.PlayerStartTag;
		if (SaveGameManager)
		{
			SaveGameManager->AutoSave_LevelTransition(SaveGameParams);
		}
	}
	LevelTransition = NewObject<UAuraLevelTransition>(this, UAuraLevelTransition::StaticClass());
	LevelTransition->Initialize(Params);
	LevelTransition->OnComplete.AddWeakLambda(this, [&](const UWorld* World)
	{
		LevelTransition->ConditionalBeginDestroy();
		LevelTransition = nullptr;
		OnLevelTransitionComplete.Broadcast();
	});
	const FAuraMapConfig& MapConfig = GetMapConfigByMapAssetName(Params.MapAssetName);
	UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), MapConfig.Map);
}

bool UAuraLevelManager::IsTransitioningLevels() const
{
	return LevelTransition != nullptr;
}


FAuraMapConfig UAuraLevelManager::GetMapConfigByDisplayName(const FString& MapDisplayName) const
{
	for (const FAuraMapConfig& MapConfig : MapConfigs)
	{
		if (MapConfig.MapDisplayName.Equals(MapDisplayName))
		{
			return MapConfig;
		}
	}
	FAuraMapConfig InvalidConfig = FAuraMapConfig();
	return InvalidConfig;
}

FAuraMapConfig UAuraLevelManager::GetMapConfigByMapAssetName(const FString& MapAssetName) const
{
	for (const FAuraMapConfig& MapConfig : MapConfigs)
	{
		if (MapConfig.Map.GetAssetName().Equals(MapAssetName))
		{
			return MapConfig;
		}
	}
	FAuraMapConfig InvalidConfig = FAuraMapConfig();
	return InvalidConfig;
}
