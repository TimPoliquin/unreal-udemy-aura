// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LevelGameInstanceSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FAuraMapConfig
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString MapDisplayName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName DefaultPlayerStartTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 DefaultPlayerLevel = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UWorld> Map;

	bool IsValid() const
	{
		return !MapDisplayName.IsEmpty() && !Map.IsNull();
	}
};

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class AURA_API ULevelGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	TSoftObjectPtr<UWorld> GetMapFromMapDisplayName(const FString& MapDisplayName) const;
	FString GetMapNameFromMapAssetName(const FString& MapAssetName) const;
	void LoadMap(const UObject* WorldContextObject, const FString& MapDisplayName) const;
	FString GetDefaultMapName() const;
	FString GetDefaultMapAssetName() const;
	FName GetDefaultPlayerStartTag(const FString& MapDisplayName) const;
	int32 GetDefaultPlayerLevel(const FString& MapDisplayName) const;
	FName GetCurrentPlayerStartTag(const UObject* WorldContextObject, const bool bFallbackToDefault) const;
	void SetCurrentPlayerStartTag(const FName& InPlayerStartTag);

	static ULevelGameInstanceSubsystem* Get(const UObject* WorldContextObject);

protected:
	UPROPERTY(EditDefaultsOnly, Category="Maps")
	FString DefaultMapName;
	UPROPERTY(EditDefaultsOnly, Category="Maps")
	TArray<FAuraMapConfig> MapConfigs = TArray<FAuraMapConfig>();

	FName CurrentPlayerStartTag = FName();

private:
	FAuraMapConfig GetMapConfigByDisplayName(const FString& MapDisplayName) const;
	FAuraMapConfig GetMapConfigByMapAssetName(const FString& MapAssetName) const;
};
