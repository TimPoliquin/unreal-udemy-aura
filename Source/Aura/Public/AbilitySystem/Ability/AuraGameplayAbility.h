// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Utils/RichTextMacros.h"
#include "AuraGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	FORCEINLINE TArray<FGameplayTag> GetStartupInputTag() const
	{
		return StartupInputTags;
	}

	virtual FString GetAbilityName() const
	{
		return AbilityName.ToString();
	}

	virtual FString GetDescription(const int32 AbilityLevel) const
	{
		return FString(TEXT(RICH_DEFAULT("Unimplemented")));
	};

	static FString GetLockedDescription(const int32 LevelRequirement)
	{
		return FString::Printf(
			TEXT(
				RICH_DEFAULT("Spell Locked Until Level") RICH_LEVEL("%d")
			),
			LevelRequirement
		);
	}

private:
	UPROPERTY(EditDefaultsOnly)
	FName AbilityName;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TArray<FGameplayTag> StartupInputTags;
};
