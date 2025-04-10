// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraItemTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraItemBlueprintLibrary.generated.h"

struct FAuraItemDefinition;
/**
 * 
 */
UCLASS()
class AURA_API UAuraItemBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static FAuraItemDefinition GetItemDefinitionByItemType(
		const UObject* WorldContextObject,
		const EAuraItemType ItemType
	);
};
