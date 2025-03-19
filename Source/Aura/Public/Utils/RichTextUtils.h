// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "RichTextUtils.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API URichTextUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(
		BlueprintPure,
		Category= "AuraRichTextUtils|TextStyles",
		meta = (DefaultToSelf = "WorldContextObject")
	)
	static FString Default(const FString& Text)
	{
		return FString::Printf(TEXT("<Default>%s</>"), *Text);
	}

	UFUNCTION(
		BlueprintPure,
		Category= "AuraRichTextUtils|TextStyles",
		meta = (DefaultToSelf = "WorldContextObject")
	)
	static FString Damage(const int32 Damage)
	{
		return FString::Printf(TEXT("<Damage>%d</>"), Damage);
	}

	UFUNCTION(
		BlueprintPure,
		Category= "AuraRichTextUtils|TextStyles",
		meta = (DefaultToSelf = "WorldContextObject")
	)
	static FString Level(const int32 InLevel)
	{
		return FString::Printf(TEXT("<Level>%d</>"), InLevel);
	}

	UFUNCTION(
		BlueprintPure,
		Category= "AuraRichTextUtils|TextStyles",
		meta = (DefaultToSelf = "WorldContextObject")
	)
	static FString Title(const FString& InTitle)
	{
		return FString::Printf(TEXT("<Title>%s</>"), *InTitle);
	}

	UFUNCTION(
		BlueprintPure,
		Category= "AuraRichTextUtils|TextStyles",
		meta = (DefaultToSelf = "WorldContextObject")
	)
	static FString Small(const FString& InSmall)
	{
		return FString::Printf(TEXT("<Small>%s</>"), *InSmall);
	}
};
