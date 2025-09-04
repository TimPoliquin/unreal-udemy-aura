#pragma once

#include "AuraSaveGameTypes.generated.h"

USTRUCT(BlueprintType)
struct AURA_API FComponentSaveData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FString ComponentName;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FString ComponentClass;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	TArray<uint8> ComponentData;

	FComponentSaveData()
	{
		ComponentName = "";
		ComponentClass = "";
	}
};

USTRUCT(BlueprintType)
struct AURA_API FActorSaveData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FString ActorName;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FString ActorClass;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FTransform ActorTransform;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	bool bShouldAutoSpawn = false;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	TArray<uint8> ActorData;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	TArray<FComponentSaveData> ComponentsData;

	FActorSaveData()
	{
		ActorName = "";
		ActorClass = "";
		ActorTransform = FTransform::Identity;
	}

	bool IsValid() const
	{
		return !ActorName.IsEmpty() && !ActorClass.IsEmpty();
	}
};

USTRUCT(BlueprintType)
struct AURA_API FWorldSaveData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FString WorldName;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	TMap<FString, FString> WorldProperties;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	TArray<FActorSaveData> ActorsData;

	FWorldSaveData()
	{
		WorldName = "";
	}

	bool IsValid() const
	{
		return !WorldName.IsEmpty();
	}

	void ClearAllData()
	{
		WorldProperties.Empty();
		ActorsData.Empty();
	}
};

USTRUCT(BlueprintType)
struct AURA_API FMetaSaveData
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FString PlayerName;
	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FDateTime SaveTimestamp;
	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FString MapName;
	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FName PlayerStartTag;

	FMetaSaveData()
	{
		PlayerName = "";
		SaveTimestamp = FDateTime::Now();
		MapName = "";
		PlayerStartTag = "";
	}
};

USTRUCT(BlueprintType)
struct AURA_API FGlobalSaveData
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FActorSaveData GameStateSaveData;
	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	TArray<FActorSaveData> PlayerSaveData;
};
