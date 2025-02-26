// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ArrayUtils.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UArrayUtils : public UObject
{
	GENERATED_BODY()

public:
	template <typename T>
	static T GetRandomElement(const TArray<T>& Array);
};

template <typename T>
T UArrayUtils::GetRandomElement(const TArray<T>& Array)
{
	if (Array.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Attempting to get random element from empty array!"));
		return T();
	}
	return Array[FMath::RandRange(0, Array.Num() - 1)];
}
