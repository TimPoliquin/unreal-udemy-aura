// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AuraCharacterGameInstanceSubsystem.generated.h"

class UGameplayEffect;
class UAbilityInfo;
class UCharacterClassInfo;
/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class AURA_API UAuraCharacterGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UAuraCharacterGameInstanceSubsystem* Get(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UAbilityInfo* GetAbilityInfo() const { return AbilityInfo; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UCharacterClassInfo* GetCharacterClassInfo() const { return CharacterClassInfo; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TSubclassOf<UGameplayEffect> GetDefaultInteractEffectClass() const { return DefaultInteractEffectClass; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetDefaultPlayerLevel() const { return DefaultPlayerLevel; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 DefaultPlayerLevel = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ability Info")
	TObjectPtr<UAbilityInfo> AbilityInfo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ability Info")
	TSubclassOf<UGameplayEffect> DefaultInteractEffectClass;
};
