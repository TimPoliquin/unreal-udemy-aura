// Copyright Alien Shores


#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/AuraLogChannels.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/AuraPlayerController.h"
#include "Tags/AuraGameplayTags.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	// Add primary attributes to attribute map
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Strength, GetStrengthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Resilience, GetResilienceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Vigor, GetVigorAttribute);
	// Add secondary attributes to attribute map
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Armor, GetArmorAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_BlockChance, GetBlockChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_HealthRegeneration, GetHealthRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ManaRegeneration, GetManaRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxMana, GetMaxManaAttribute);
	// add resistances to attribute map
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Arcane, GetResistance_ArcaneAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Fire, GetResistance_FireAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Lightning, GetResistance_LightningAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Physical, GetResistance_PhysicalAttribute);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// primary attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Vigor, COND_None, REPNOTIFY_Always);
	// secondary attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	// vital attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	// Resistance attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resistance_Arcane, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resistance_Fire, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resistance_Lightning, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resistance_Physical, COND_None, REPNOTIFY_Always);
}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	// This does not actually clamp the health value - it only clamps the modifier.
	// Attribute must also be clamped in PostAttributeChange.
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}

void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties Props;
	SetEffectProperties(Data, Props);
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
	else if (Data.EvaluatedData.Attribute == GetMeta_IncomingDamageAttribute())
	{
		HandleIncomingDamage(Props);
	}
	else if (Data.EvaluatedData.Attribute == GetMeta_IncomingXPAttribute())
	{
		HandleIncomingXP(Props);
	}
}

void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.Source.AbilitySystemComponent = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	Props.Source.AvatarActor = Props.Source.AbilitySystemComponent->GetAvatarActor();
	Props.Source.Controller = Props.Source.AbilitySystemComponent->AbilityActorInfo->PlayerController.Get();
	if (Props.Source.Controller == nullptr && Props.Source.AvatarActor != nullptr)
	{
		if (const APawn* Pawn = Cast<APawn>(Props.Source.AvatarActor))
		{
			Props.Source.Controller = Pawn->GetController();
		}
	}
	if (Props.Source.Controller)
	{
		Props.Source.Character = Props.Source.Controller->GetCharacter();
	}

	Props.Target.AvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
	Props.Target.Controller = Data.Target.AbilityActorInfo->PlayerController.Get();
	Props.Target.Character = Cast<ACharacter>(Props.Target.AvatarActor);
	Props.Target.AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
		Props.Target.AvatarActor
	);
}

void UAuraAttributeSet::HandleIncomingDamage(const FEffectProperties& Props)
{
	const float IncomingDamage = GetMeta_IncomingDamage();
	SetMeta_IncomingDamage(0.f);
	if (IncomingDamage > 0.f)
	{
		const float NewHealth = GetHealth() - IncomingDamage;
		const bool bFatal = NewHealth <= 0.f;
		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
		if (!bFatal)
		{
			FGameplayTagContainer TagContainer;
			TagContainer.AddTag(FAuraGameplayTags::Get().Effect_HitReact);
			Props.Target.AbilitySystemComponent->TryActivateAbilitiesByTag(
				TagContainer
			);
		}
		else
		{
			if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.Target.AvatarActor))
			{
				CombatInterface->Die();
			}
			SendXPEvent(Props);
		}
		ShowDamageText(Props, IncomingDamage);
	}
}

void UAuraAttributeSet::HandleIncomingXP(const FEffectProperties& Props)
{
	const float IncomingXP = GetMeta_IncomingXP();
	SetMeta_IncomingXP(0.f);
	// TODO - Determine if we need to level up!
	IPlayerInterface::AddToXP(Props.Source.AvatarActor, IncomingXP);
}


void UAuraAttributeSet::ShowDamageText(const FEffectProperties& Props, const float& IncomingDamage) const
{
	if (Props.Source.Character != Props.Target.Character)
	{
		if (AAuraPlayerController* PlayerController = Cast<AAuraPlayerController>(
			Props.Source.Character->GetController()
		))
		{
			// show damage on enemy
			PlayerController->ShowDamageNumber(
				Props.Target.Character,
				IncomingDamage,
				UAuraAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle),
				UAuraAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle)
			);
		}
		else if (AAuraPlayerController* PlayerController = Cast<AAuraPlayerController>(
			Props.Target.Character->GetController()
		))
		{
			// show damage on player
			PlayerController->ShowDamageNumber(
				Props.Target.Character,
				IncomingDamage,
				UAuraAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle),
				UAuraAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle)
			);
		}
	}
}

void UAuraAttributeSet::SendXPEvent(const FEffectProperties& Props) const
{
	const int32 XPReward = ICombatInterface::Execute_GetXPReward(Props.Target.AvatarActor);
	FGameplayEventData Payload;
	Payload.EventTag = FAuraGameplayTags::Get().Attributes_Meta_IncomingXP;
	Payload.EventMagnitude = XPReward;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		Props.Source.AvatarActor,
		Payload.EventTag,
		Payload
	);
}
