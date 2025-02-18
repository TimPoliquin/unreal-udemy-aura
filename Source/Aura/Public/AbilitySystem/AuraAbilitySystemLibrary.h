// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemLibrary.generated.h"

class UGameplayEffect;
class UAbilitySystemComponent;
enum class ECharacterClass : uint8;
class UAttributeMenuWidgetController;
class AAuraHUD;
struct FWidgetControllerParams;
class UOverlayWidgetController;
/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category= "AuraAbilitySystemLibrary|WidgetController")
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);
	UFUNCTION(BlueprintPure, Category= "AuraAbilitySystemLibrary|WidgetController")
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttributes(
		const UObject* WorldContextObject,
		ECharacterClass CharacterClass,
		const float Level,
		UAbilitySystemComponent* AbilitySystemComponent
	);

private:
	static void GetWidgetControllerParams(
		const UObject* WorldContextObject,
		FWidgetControllerParams& FWidgetControllerParams
	);
	static AAuraHUD* GetAuraHUD(const UObject* WorldContextObject);

	static void ApplyGameplayEffectSpec(
		UAbilitySystemComponent* AbilitySystemComponent,
		const TSubclassOf<UGameplayEffect>& GameplayEffectClass,
		const float Level
	);
};
