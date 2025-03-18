// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemLibrary.generated.h"

class UAbilityInfo;
class USpellMenuWidgetController;
struct FGameplayAbilitySpec;
class UGameplayAbility;
struct FGameplayEffectSpecHandle;
struct FGameplayEffectContextHandle;
class UCharacterClassInfo;
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
	UFUNCTION(
		BlueprintPure,
		Category= "AuraAbilitySystemLibrary|WidgetController",
		meta = (DefaultToSelf = "WorldContextObject")
	)
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);
	UFUNCTION(
		BlueprintPure,
		Category= "AuraAbilitySystemLibrary|WidgetController",
		meta = (DefaultToSelf = "WorldContextObject")
	)
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);
	UFUNCTION(
		BlueprintPure,
		Category= "AuraAbilitySystemLibrary|WidgetController",
		meta = (DefaultToSelf = "WorldContextObject")
	)
	static USpellMenuWidgetController* GetSpellMenuWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category= "AuraAbilitySystemLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttributes(
		const UObject* WorldContextObject,
		ECharacterClass CharacterClass,
		const float Level,
		UAbilitySystemComponent* AbilitySystemComponent
	);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|CharacterClassDefaults")
	static void GrantStartupAbilities(
		const UObject* WorldContextObject,
		UAbilitySystemComponent* AbilitySystemComponent,
		ECharacterClass CharacterClass,
		int Level
	);
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|CharacterClassDefaults")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|AbilityInfo")
	static UAbilityInfo* GetAbilityInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static bool IsInfiniteEffect(const FGameplayEffectSpecHandle& SpecHandle);

	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static bool IsInstantEffect(const FGameplayEffectSpecHandle& SpecHandle);

	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool InIsBlocked);
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool InIsCriticalHit);

	static int GetCharacterLevel(UAbilitySystemComponent* AbilitySystemComponent);
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetStatusTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static int32 GetXPReward(const UObject* WorldContextObject, const ECharacterClass& CharacterClass, int32 Level);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayMechanics")
	static void GetLivePlayersWithinRadius(
		const UObject* WorldContextObject,
		const TArray<AActor*>& ActorsToIgnore,
		const TArray<FName>& TagsToIgnore,
		const FVector& SphereOrigin,
		float Radius,
		TArray<AActor*>& OutOverlappingActors
	);

private:
	static bool GetWidgetControllerParams(
		const UObject* WorldContextObject,
		FWidgetControllerParams& FWidgetControllerParams
	);
	static AAuraHUD* GetAuraHUD(const UObject* WorldContextObject);

	static void ApplyGameplayEffectSpec(
		UAbilitySystemComponent* AbilitySystemComponent,
		const TSubclassOf<UGameplayEffect>& GameplayEffectClass,
		const float Level
	);

	static void GrantAbilities(
		UAbilitySystemComponent* AbilitySystemComponent,
		const TArray<TSubclassOf<UGameplayAbility>>& Abilities,
		int Level
	);
};
