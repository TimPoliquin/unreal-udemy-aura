// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Utils/RichTextUtils.h"
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
		return URichTextUtils::Default(TEXT("Unimplemented"));
	};

	static FString GetLockedDescription(const int32 LevelRequirement)
	{
		return URichTextUtils::Default(TEXT("Spell Locked Until Level %s")) + URichTextUtils::Level(LevelRequirement);
	}

private:
	UPROPERTY(EditDefaultsOnly)
	FName AbilityName;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TArray<FGameplayTag> StartupInputTags;
};
