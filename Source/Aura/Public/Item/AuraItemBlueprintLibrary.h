// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraItemTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraItemBlueprintLibrary.generated.h"

struct FAuraFishCatch;
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
		const FGameplayTag& ItemType
	);
	UFUNCTION(BlueprintCallable, Category="AuraItemBlueprintLibrary|Items")
	static FString GetItemNameByItemType(const UObject* WorldContextObject, const FGameplayTag& ItemType);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AuraItemBlueprintLibrary|Messages")
	static FString Substitute(const FString& Message, const FMessageSubstitutions& MessageSubstitutions);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AuraItemBlueprintLibrary|Messages")
	static UTexture2D* SubstituteMessageIcon(UTexture2D* MessageIcon, const FMessageSubstitutions& MessageSubstitutions);

	static FAuraFishCatch ToFishCatch(const UObject* WorldContextObject, const FGameplayTag& FishType);
};
