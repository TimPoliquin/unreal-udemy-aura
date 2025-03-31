// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemLibrary.generated.h"

DECLARE_DELEGATE_OneParam(FMakeEffectSpecSignature, FGameplayEffectSpecHandle&);

struct FDamageEffectParams;
class UAuraAbilitySystemComponent;
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

	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static bool IsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static float GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static float GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static FVector GetRadialDamageOrigin(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static void SetIsRadialDamage(FGameplayEffectContextHandle& EffectContextHandle, bool InIsRadialDamage);
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static void SetRadialDamageInnerRadius(
		FGameplayEffectContextHandle& EffectContextHandle,
		float InRadialDamageInnerRadius
	);
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static void SetRadialDamageOuterRadius(
		FGameplayEffectContextHandle& EffectContextHandle,
		float InRadialDamageOuterRadius
	);
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static void SetRadialDamageOrigin(
		FGameplayEffectContextHandle& EffectContextHandle,
		const FVector& InRadialDamageOrigin
	);

	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static bool IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static float GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static float GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static float GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static FGameplayTag GetDebuffTypeTag(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static FGameplayTag GetDamageTypeTag(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static FVector GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static FVector GetKnockbackVector(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool InIsBlocked);
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool InIsCriticalHit);
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static void SetDebuff(
		FGameplayEffectContextHandle& EffectContextHandle,
		FGameplayTag& DebuffTypeTag,
		float DebuffDamage,
		float DebuffDuration,
		float DebuffFrequency
	);
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static void SetIsSuccessfulDebuff(
		UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,
		bool InIsSuccessfulDebuff
	);
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static void SetDebuffDamage(
		UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,
		float InDebuffDamage
	);
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static void SetDebuffDuration(
		UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,
		float InDebuffDuration
	);
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static void SetDebuffFrequency(
		UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,
		float InDebuffFrequency
	);
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static void SetDebuffTypeTag(
		UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,
		const FGameplayTag& InDebuffTypeTag
	);
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static void SetDamageTypeTag(
		UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,
		const FGameplayTag& InDamageTypeTag
	);
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static void SetDeathImpulse(
		UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,
		const FVector& InDeathImpulse
	);
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static void SetKnockbackVector(
		UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,
		const FVector& InKnockbackVector
	);

	static int GetCharacterLevel(UAbilitySystemComponent* AbilitySystemComponent);
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static TArray<FGameplayTag> GetInputTagsFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetStatusTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetStatusTagByAbilityTag(
		UAuraAbilitySystemComponent* AbilitySystemComponent,
		const FGameplayTag& AbilityTag
	);
	static FGameplayTag GetInputTagByAbilityTag(
		UAuraAbilitySystemComponent* AbilitySystemComponent,
		const FGameplayTag& AbilityTag
	);

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

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayMechanics")
	static void GetClosestActors(
		int32 MaxCount,
		const FVector& Location,
		const TArray<AActor*>& Actors,
		TArray<AActor*>& OutActors
	);

	static bool CanEquipAbility(UAuraAbilitySystemComponent* AbilitySystemComponent, const FGameplayTag& AbilityTag);
	static bool AbilityHasSlotTag(const FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& SlotTag);
	static bool AbilityHasAnySlot(const FGameplayAbilitySpec& AbilitySpec);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffect")
	static FGameplayEffectContextHandle ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayMechanics")
	static TArray<FRotator> EvenlySpacedRotators(
		const FVector& Forward,
		const FVector& Axis,
		const float Spread,
		const int32 Count
	);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayMechanics")
	static TArray<FVector> EvenlyRotatedVectors(
		const FVector& Forward,
		const FVector& Axis,
		const float Spread,
		const int32 Count
	);
	static FPredictionKey GetPredictionKeyFromAbilitySpec(const FGameplayAbilitySpec& AbilitySpec);
	static void ApplyGameplayEffectSpec(
		const UAbilitySystemComponent* SourceAbilitySystemComponent,
		UAbilitySystemComponent* TargetAbilitySystemComponent,
		const TSubclassOf<UGameplayEffect>& GameplayEffectClass,
		float Level,
		const FMakeEffectSpecSignature* SetPropsOnSpecCallback = nullptr
	);

	static bool IsPassiveAbility(const UObject* WorldContextObject, const FGameplayAbilitySpec& AbilitySpec);

private:
	static bool GetWidgetControllerParams(
		const UObject* WorldContextObject,
		FWidgetControllerParams& FWidgetControllerParams
	);
	static AAuraHUD* GetAuraHUD(const UObject* WorldContextObject);

	static void GrantAbilities(
		UAbilitySystemComponent* AbilitySystemComponent,
		const TArray<TSubclassOf<UGameplayAbility>>& Abilities,
		int Level
	);
};
