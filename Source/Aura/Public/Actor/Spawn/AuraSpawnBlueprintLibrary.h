// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraSpawnBlueprintLibrary.generated.h"

USTRUCT(BlueprintType)
struct FAuraSpawnParams
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<AActor> SpawnClass;
	UPROPERTY(BlueprintReadOnly)
	FTransform SpawnTransform;
};

/**
 * 
 */
UCLASS()
class AURA_API UAuraSpawnBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static TArray<FTransform> GenerateSpawnLocations(const FVector& Center, const float Radius, const int32 NumSpawns);
	UFUNCTION(BlueprintCallable)
	static FAuraSpawnParams Pop(UPARAM(ref)
		TArray<FAuraSpawnParams>& SpawnParams);
};
