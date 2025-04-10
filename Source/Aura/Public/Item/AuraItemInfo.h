// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraItemTypes.h"
#include "Engine/DataAsset.h"
#include "AuraItemInfo.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraItemInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	FAuraItemDefinition FindItemByName(const FString& ItemName) const;
	FAuraItemDefinition FindItemByItemType(const EAuraItemType ItemType) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	TArray<FAuraItemDefinition> ItemInfo;
};
