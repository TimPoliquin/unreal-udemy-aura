// Copyright Alien Shores


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystemComponent.h"
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
	FWidgetControllerParams WidgetControllerParams;
	GetWidgetControllerParams(WorldContextObject, WidgetControllerParams);
	if (AAuraHUD* AuraHUD = GetAuraHUD(WorldContextObject))
	{
		return AuraHUD->GetOverlayWidgetController(WidgetControllerParams);
	}
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject
)
{
	FWidgetControllerParams WidgetControllerParams;
	GetWidgetControllerParams(WorldContextObject, WidgetControllerParams);
	if (AAuraHUD* AuraHUD = GetAuraHUD(WorldContextObject))
	{
		return AuraHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
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
	if (AbilitySystemComponent)
	{
		if (const ICombatInterface* CombatInterface = Cast<ICombatInterface>(AbilitySystemComponent->GetAvatarActor()))
		{
			return CombatInterface->GetCharacterLevel();
		}
	}
	return 0;
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

void UAuraAbilitySystemLibrary::GetWidgetControllerParams(
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
	}
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
