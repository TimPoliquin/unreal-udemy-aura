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
	/**
	 *Returns <code>true</code> if Source contains all elements in the Elements list
	 */
	template <typename T>
	static bool ContainsAll(const TArray<T>& Source, const TArray<T>& Elements);
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

template <typename T>
bool UArrayUtils::ContainsAll(const TArray<T>& Source, const TArray<T>& Elements)
{
	for (const T& Element : Elements)
	{
		if (!Source.Contains(Element))
		{
			return false;
		}
	}
	return true;
}
