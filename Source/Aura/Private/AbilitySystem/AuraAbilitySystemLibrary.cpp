// Copyright Alien Shores


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemTypes.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Aura/AuraLogChannels.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "Tags/AuraGameplayTags.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "Utils/TagUtils.h"

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if (FWidgetControllerParams WidgetControllerParams; GetWidgetControllerParams(
		WorldContextObject,
		WidgetControllerParams
	))
	{
		if (AAuraHUD* AuraHUD = GetAuraHUD(WorldContextObject))
		{
			return AuraHUD->GetOverlayWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject
)
{
	if (FWidgetControllerParams WidgetControllerParams; GetWidgetControllerParams(
		WorldContextObject,
		WidgetControllerParams
	))
	{
		if (AAuraHUD* AuraHUD = GetAuraHUD(WorldContextObject))
		{
			return AuraHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

USpellMenuWidgetController* UAuraAbilitySystemLibrary::GetSpellMenuWidgetController(
	const UObject* WorldContextObject
)
{
	if (FWidgetControllerParams WidgetControllerParams; GetWidgetControllerParams(
		WorldContextObject,
		WidgetControllerParams
	))
	{
		if (AAuraHUD* AuraHUD = GetAuraHUD(WorldContextObject))
		{
			return AuraHUD->GetSpellMenuWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(
	const UObject* WorldContextObject,
	const ECharacterClass CharacterClass,
	const float Level,
	UAbilitySystemComponent* AbilitySystemComponent
)
{
	if (const AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		const UCharacterClassInfo* ClassInfo = GameMode->GetCharacterClassInfo();
		const FCharacterClassDefaultInfo DefaultInfo = ClassInfo->GetClassDefaultInfo(CharacterClass);
		ApplyGameplayEffectSpec(AbilitySystemComponent, DefaultInfo.PrimaryAttributes, Level);
		ApplyGameplayEffectSpec(AbilitySystemComponent, ClassInfo->SecondaryAttributes, Level);
		ApplyGameplayEffectSpec(AbilitySystemComponent, ClassInfo->VitalAttributes, Level);
	}
}

void UAuraAbilitySystemLibrary::GrantStartupAbilities(
	const UObject* WorldContextObject,
	UAbilitySystemComponent* AbilitySystemComponent,
	const ECharacterClass CharacterClass,
	const int Level
)
{
	if (const AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		UCharacterClassInfo* CharacterClassInfo = GameMode->GetCharacterClassInfo();
		GrantAbilities(AbilitySystemComponent, CharacterClassInfo->CommonAbilities, 1);
		GrantAbilities(
			AbilitySystemComponent,
			CharacterClassInfo->CharacterClassInformation[CharacterClass].
			ClassAbilities,
			Level
		);
	}
}

void UAuraAbilitySystemLibrary::GrantAbilities(
	UAbilitySystemComponent* AbilitySystemComponent,
	const TArray<TSubclassOf<UGameplayAbility>>& Abilities,
	const int Level
)
{
	for (const TSubclassOf AbilityClass : Abilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, Level);
		AbilitySystemComponent->GiveAbility(AbilitySpec);
	}
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	if (const AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		return GameMode->GetCharacterClassInfo();
	}
	return nullptr;
}

UAbilityInfo* UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	if (const AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		return GameMode->GetAbilityInfo();
	}
	return nullptr;
}

bool UAuraAbilitySystemLibrary::IsInfiniteEffect(const FGameplayEffectSpecHandle& SpecHandle)
{
	return SpecHandle.Data.Get()->Def.Get()->DurationPolicy ==
		EGameplayEffectDurationType::Infinite;
}

bool UAuraAbilitySystemLibrary::IsInstantEffect(const FGameplayEffectSpecHandle& SpecHandle)
{
	return SpecHandle.Data.Get()->Def.Get()->DurationPolicy ==
		EGameplayEffectDurationType::Instant;
}

int UAuraAbilitySystemLibrary::GetCharacterLevel(UAbilitySystemComponent* AbilitySystemComponent)
{
	return ICombatInterface::GetCharacterLevel(AbilitySystemComponent->GetAvatarActor());
}

FGameplayTag UAuraAbilitySystemLibrary::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (const FGameplayTag& Tag : AbilitySpec.Ability.Get()->GetAssetTags())
		{
			if (Tag.MatchesTag(FAuraGameplayTags::Get().Abilities))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemLibrary::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (const FGameplayTag& Tag : AbilitySpec.GetDynamicSpecSourceTags())
		{
			if (Tag.MatchesTag(FAuraGameplayTags::Get().InputTag))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

TArray<FGameplayTag> UAuraAbilitySystemLibrary::GetInputTagsFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	TArray<FGameplayTag> InputTags;
	if (AbilitySpec.Ability)
	{
		for (const FGameplayTag& Tag : AbilitySpec.GetDynamicSpecSourceTags())
		{
			if (Tag.MatchesTag(FAuraGameplayTags::Get().InputTag))
			{
				InputTags.Add(Tag);
			}
		}
	}
	return InputTags;
}

int32 UAuraAbilitySystemLibrary::GetXPReward(
	const UObject* WorldContextObject,
	const ECharacterClass& CharacterClass,
	const int32 Level
)
{
	if (const AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		return GameMode->GetCharacterClassInfo()->GetXPReward(CharacterClass, Level);
	}
	UE_LOG(LogAura, Error, TEXT("Game mode is not set to AuraGameMode!"))
	return 0;
}

void UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
	const UObject* WorldContextObject,
	const TArray<AActor*>& ActorsToIgnore,
	const TArray<FName>& TagsToIgnore,
	const FVector& SphereOrigin,
	const float Radius,
	TArray<AActor*>& OutOverlappingActors
)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);
	if (const UWorld* World = GEngine->GetWorldFromContextObject(
		WorldContextObject,
		EGetWorldErrorMode::LogAndReturnNull
	))
	{
		TArray<FOverlapResult> Overlaps;
		World->OverlapMultiByObjectType(
			Overlaps,
			SphereOrigin,
			FQuat::Identity,
			FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects),
			FCollisionShape::MakeSphere(Radius),
			SphereParams
		);
		for (const FOverlapResult& Overlap : Overlaps)
		{
			AActor* OverlapActor = Overlap.GetActor();
			// skip actor if it has any of the tags in the ignore list
			if (TagUtils::HasAnyTag(OverlapActor, TagsToIgnore))
			{
				continue;
			}
			if (ICombatInterface::IsAlive(Overlap.GetActor()))
			{
				OutOverlappingActors.Add(OverlapActor);
			}
		}
	}
}

bool UAuraAbilitySystemLibrary::CanEquipAbility(
	UAuraAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayTag& AbilityTag
)
{
	const FGameplayTag StatusTag = GetStatusTagByAbilityTag(AbilitySystemComponent, AbilityTag);
	FGameplayTagContainer EquippableStatuses;
	EquippableStatuses.AddTag(FAuraGameplayTags::Get().Abilities_Status_Unlocked);
	EquippableStatuses.AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
	return StatusTag.MatchesAny(EquippableStatuses);
}

bool UAuraAbilitySystemLibrary::AbilityHasSlotTag(const FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& SlotTag)
{
	for (const FGameplayTag& Tag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (Tag.MatchesTagExact(SlotTag))
		{
			return true;
		}
	}
	return false;
}

FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams)
{
	checkf(
		DamageEffectParams.TargetAbilitySystemComponent,
		TEXT("No target ability system set on Damage Effect Params!")
	)
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	const AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	FGameplayEffectContextHandle EffectContextHandle = DamageEffectParams.SourceAbilitySystemComponent->
	                                                                      MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceAvatarActor);
	const FGameplayEffectSpecHandle SpecHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeOutgoingSpec(
		DamageEffectParams.DamageGameplayEffectClass,
		DamageEffectParams.AbilityLevel,
		EffectContextHandle
	);
	SetDeathImpulse(EffectContextHandle, DamageEffectParams.DeathImpulse);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		DamageEffectParams.DamageType,
		DamageEffectParams.BaseDamage
	);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		GameplayTags.Debuff_Stat_Chance,
		DamageEffectParams.DebuffChance
	);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		GameplayTags.Debuff_Stat_Damage,
		DamageEffectParams.DebuffDamage
	);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		GameplayTags.Debuff_Stat_Duration,
		DamageEffectParams.DebuffDuration
	);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		GameplayTags.Debuff_Stat_Frequency,
		DamageEffectParams.DebuffFrequency
	);
	DamageEffectParams.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	return EffectContextHandle;
}

FGameplayTag UAuraAbilitySystemLibrary::GetStatusTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (const FGameplayTag& Tag : AbilitySpec.GetDynamicSpecSourceTags())
		{
			if (Tag.MatchesTag(FAuraGameplayTags::Get().Abilities_Status))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemLibrary::GetStatusTagByAbilityTag(
	UAuraAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayTag& AbilityTag
)
{
	if (const FGameplayAbilitySpec* Spec = AbilitySystemComponent->GetSpecFromAbilityTag(AbilityTag))
	{
		return GetStatusTagFromSpec(*Spec);
	}
	return FAuraGameplayTags::Get().Abilities_Status_Locked;
}

FGameplayTag UAuraAbilitySystemLibrary::GetInputTagByAbilityTag(
	UAuraAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayTag& AbilityTag
)
{
	if (const FGameplayAbilitySpec* Spec = AbilitySystemComponent->GetSpecFromAbilityTag(AbilityTag))
	{
		return GetInputTagFromSpec(*Spec);
	}
	return FGameplayTag();
}

bool UAuraAbilitySystemLibrary::GetWidgetControllerParams(
	const UObject* WorldContextObject,
	FWidgetControllerParams& FWidgetControllerParams
)
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		AAuraPlayerState* PlayerState = PlayerController->GetPlayerState<AAuraPlayerState>();
		UAbilitySystemComponent* AbilitySystemComponent = PlayerState->GetAbilitySystemComponent();
		UAttributeSet* AttributeSet = PlayerState->GetAttributeSet();
		FWidgetControllerParams.PlayerController = PlayerController;
		FWidgetControllerParams.PlayerState = PlayerState;
		FWidgetControllerParams.AbilitySystemComponent = AbilitySystemComponent;
		FWidgetControllerParams.AttributeSet = AttributeSet;
		return true;
	}
	return false;
}

AAuraHUD* UAuraAbilitySystemLibrary::GetAuraHUD(const UObject* WorldContextObject)
{
	if (const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		return Cast<AAuraHUD>(PlayerController->GetHUD());
	}
	return nullptr;
}

void UAuraAbilitySystemLibrary::ApplyGameplayEffectSpec(
	UAbilitySystemComponent* AbilitySystemComponent,
	const TSubclassOf<UGameplayEffect>& GameplayEffectClass,
	const float Level
)
{
	FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
	ContextHandle.AddSourceObject(AbilitySystemComponent->GetAvatarActor());
	const FGameplayEffectSpecHandle EffectSpec = AbilitySystemComponent->MakeOutgoingSpec(
		GameplayEffectClass,
		Level,
		ContextHandle
	);
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());
}

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsBlockedHit();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsCriticalHit();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsSuccessfullDebuff();
	}
	return false;
}

float UAuraAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDebuffDamage();
	}
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDebuffDuration();
	}
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDebuffFrequency();
	}
	return 0.f;
}

FGameplayTag UAuraAbilitySystemLibrary::GetDebuffTypeTag(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return *AuraEffectContext->GetDebuffTypeTag();
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemLibrary::GetDamageTypeTag(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return *AuraEffectContext->GetDamageTypeTag();
	}
	return FGameplayTag();
}

FVector UAuraAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetDeathImpulse();
	}
	return FVector::ZeroVector;
}

void UAuraAbilitySystemLibrary::SetIsBlockedHit(
	FGameplayEffectContextHandle& EffectContextHandle,
	const bool InIsBlocked
)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsBlockedHit(InIsBlocked);
	}
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(
	FGameplayEffectContextHandle& EffectContextHandle,
	bool InIsCriticalHit
)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsCriticalHit(InIsCriticalHit);
	}
}

void UAuraAbilitySystemLibrary::SetDebuff(
	FGameplayEffectContextHandle& EffectContextHandle,
	FGameplayTag& DebuffTypeTag,
	float DebuffDamage,
	float DebuffDuration,
	float DebuffFrequency
)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsSuccessfulDebuff(true);
		AuraEffectContext->SetDebuffDamage(DebuffDamage);
		AuraEffectContext->SetDebuffDuration(DebuffDuration);
		AuraEffectContext->SetDebuffFrequency(DebuffFrequency);
		AuraEffectContext->SetDebuffTypeTag(MakeShared<FGameplayTag>(DebuffTypeTag));
	}
}

void UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(
	FGameplayEffectContextHandle& EffectContextHandle,
	bool InIsSuccessfulDebuff
)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsSuccessfulDebuff(InIsSuccessfulDebuff);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDamage(
	FGameplayEffectContextHandle& EffectContextHandle,
	float InDebuffDamage
)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDebuffDamage(InDebuffDamage);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDuration(
	FGameplayEffectContextHandle& EffectContextHandle,
	float InDebuffDuration
)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDebuffDuration(InDebuffDuration);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffFrequency(
	FGameplayEffectContextHandle& EffectContextHandle,
	float InDebuffFrequency
)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDebuffFrequency(InDebuffFrequency);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffTypeTag(
	FGameplayEffectContextHandle& EffectContextHandle,
	const FGameplayTag& InDebuffTypeTag
)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDebuffTypeTag(MakeShared<FGameplayTag>(InDebuffTypeTag));
	}
}

void UAuraAbilitySystemLibrary::SetDamageTypeTag(
	FGameplayEffectContextHandle& EffectContextHandle,
	const FGameplayTag& InDamageTypeTag
)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDamageTypeTag(MakeShared<FGameplayTag>(InDamageTypeTag));
	}
}

void UAuraAbilitySystemLibrary::SetDeathImpulse(
	FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& InDeathImpulse
)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(
		EffectContextHandle.Get()))
	{
		AuraEffectContext->SetDeathImpulse(InDeathImpulse);
	}
}
