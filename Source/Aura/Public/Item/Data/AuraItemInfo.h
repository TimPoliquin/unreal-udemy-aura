// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "Item/AuraItemTypes.h"
#include "AuraItemInfo.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraItemInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	FAuraItemDefinition FindItemByItemType(const FGameplayTag& ItemType) const;
	void AddToMap(TMap<FGameplayTag, FAuraItemDefinition>& ItemDefinitionMap) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	TArray<FAuraItemDefinition> ItemInfo;
};
