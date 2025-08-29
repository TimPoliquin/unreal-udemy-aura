// Copyright Alien Shores


#include "Game/Subsystem/LevelGameInstanceSubsystem.h"

#include "Aura/AuraLogChannels.h"
#include "Kismet/GameplayStatics.h"

void ULevelGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogAura, Warning, TEXT("[%s] Initialize"), *GetName())
}

void ULevelGameInstanceSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

TSoftObjectPtr<UWorld> ULevelGameInstanceSubsystem::GetMapFromMapDisplayName(const FString& MapDisplayName) const
{
	return GetMapConfigByDisplayName(MapDisplayName).Map;
}

FString ULevelGameInstanceSubsystem::GetMapNameFromMapAssetName(const FString& MapAssetName) const
{
	return GetMapConfigByMapAssetName(MapAssetName).MapDisplayName;
}

ULevelGameInstanceSubsystem* ULevelGameInstanceSubsystem::Get(const UObject* WorldContextObject)
{
	if (const UGameInstance* GameInstance = IsValid(WorldContextObject)
		                                        ? UGameplayStatics::GetGameInstance(WorldContextObject)
		                                        : nullptr)
	{
		return GameInstance->GetSubsystem<ULevelGameInstanceSubsystem>();
	}
	return nullptr;
}

void ULevelGameInstanceSubsystem::LoadMap(const UObject* WorldContextObject, const FString& MapDisplayName) const
{
	const TSoftObjectPtr<UWorld> Map = GetMapFromMapDisplayName(MapDisplayName);
	if (!Map.IsNull())
	{
		UGameplayStatics::OpenLevelBySoftObjectPtr(WorldContextObject, Map);
	}
	else
	{
		UE_LOG(LogAura, Error, TEXT("[%s] Failed to load map by display name: %s"), *GetName(), *MapDisplayName);
		for (const FAuraMapConfig& MapConfig : MapConfigs)
		{
			UE_LOG(
				LogAura,
				Error,
				TEXT("[%s] Map: %s - "),
				*GetName(),
				*MapConfig.MapDisplayName,
				*MapConfig.Map.ToSoftObjectPath().GetAssetName()
			);
		}
	}
}


FString ULevelGameInstanceSubsystem::GetDefaultMapName() const
{
	return DefaultMapName;
}

FString ULevelGameInstanceSubsystem::GetDefaultMapAssetName() const
{
	return GetMapFromMapDisplayName(DefaultMapName).ToSoftObjectPath().GetAssetName();
}

FName ULevelGameInstanceSubsystem::GetDefaultPlayerStartTag(const FString& MapDisplayName) const
{
	return GetMapConfigByDisplayName(MapDisplayName).DefaultPlayerStartTag;
}

int32 ULevelGameInstanceSubsystem::GetDefaultPlayerLevel(const FString& MapDisplayName) const
{
	return GetMapConfigByDisplayName(MapDisplayName).DefaultPlayerLevel;
}

FName ULevelGameInstanceSubsystem::GetCurrentPlayerStartTag(
	const UObject* WorldContextObject,
	const bool bFallbackToDefault
) const
{
	if (CurrentPlayerStartTag.IsNone() && bFallbackToDefault)
	{
		const UWorld* World = WorldContextObject->GetWorld();
		FString MapName = World->GetMapName();
		MapName.RemoveFromStart(World->StreamingLevelsPrefix);
		const FAuraMapConfig& MapConfig = GetMapConfigByMapAssetName(MapName);
		return MapConfig.DefaultPlayerStartTag;
	}
	return CurrentPlayerStartTag;
}

void ULevelGameInstanceSubsystem::SetCurrentPlayerStartTag(const FName& InPlayerStartTag)
{
	CurrentPlayerStartTag = InPlayerStartTag;
}


FAuraMapConfig ULevelGameInstanceSubsystem::GetMapConfigByDisplayName(const FString& MapDisplayName) const
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

FAuraMapConfig ULevelGameInstanceSubsystem::GetMapConfigByMapAssetName(const FString& MapAssetName) const
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
