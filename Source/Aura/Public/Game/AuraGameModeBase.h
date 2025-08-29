// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/GameModeBase.h"
#include "Item/AuraItemTypes.h"
#include "Item/Data/AuraTreasureConfig.h"
#include "AuraGameModeBase.generated.h"

class UFishInfo;
class UAuraItemInfo;
class ULootTiers;
class UAuraSaveGame;
class USaveGame;
class UMVVM_LoadSlot;
class UGameplayEffect;
class UAbilityInfo;
class UCharacterClassInfo;
class UAuraGameInstance;
/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	FORCEINLINE UFishInfo* GetFishInfo() const
	{
		return FishInfo;
	}


	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	UAuraGameInstance* GetAuraGameInstance() const;
	static AAuraGameModeBase* GetAuraGameMode(const UObject* WorldContextObject);
	UAuraTreasureConfig* GetTreasureConfig() const { return TreasureConfig; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FAuraItemDefinition FindItemDefinitionByItemTag(const FGameplayTag& ItemTag);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGameplayTag GetDefaultItemPickupMessageTag() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGameplayTag GetDefaultItemUseMessageTag() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category="Items")
	TArray<UAuraItemInfo*> ItemInfos;
	UPROPERTY(EditDefaultsOnly, Category="Items", meta=(Categories="Message"))
	FGameplayTag DefaultItemPickupMessageTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly, Category="Items", meta=(Categories="Message"))
	FGameplayTag DefaultItemUsedMessageTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditDefaultsOnly, Category="Loot")
	TObjectPtr<UAuraTreasureConfig> TreasureConfig;

	UPROPERTY(EditDefaultsOnly, Category="Fishing")
	TObjectPtr<UFishInfo> FishInfo;

	UPROPERTY()
	TMap<FGameplayTag, FAuraItemDefinition> ItemDefinitions;

private:
	void InitializeItemDefinitions();
};
