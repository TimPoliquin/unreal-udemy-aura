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
	UFUNCTION(BlueprintCallable, Category="AuraItemBlueprintLibrary|Items")
	static FAuraItemDefinition GetItemDefinitionByItemType(
		const UObject* WorldContextObject,
		const EAuraItemType ItemType
	);
	UFUNCTION(BlueprintCallable, Category="AuraItemBlueprintLibrary|Items")
	static FString GetItemNameByItemType(const UObject* WorldContextObject, const EAuraItemType ItemType);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AuraItemBlueprintLibrary|Messages")
	static FString Substitute(const FString& Message, const FMessageSubstitutions& MessageSubstitutions);
};
