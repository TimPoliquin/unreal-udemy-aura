// Copyright Alien Shores


#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Net/UnrealNetwork.h"
#include "Player/AuraPlayerController.h"
#include "Tags/AuraGameplayTags.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include "Game/AuraSaveGame.h"


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
	// add passives to attribute map
	TagsToAttributes.Add(GameplayTags.Attributes_Passive_Protection, GetPassive_ProtectionAttribute);
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
	// resistance attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resistance_Arcane, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resistance_Fire, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resistance_Lightning, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resistance_Physical, COND_None, REPNOTIFY_Always);
	// passive attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Passive_Protection, COND_None, REPNOTIFY_Always);
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
	if (ICombatInterface::IsDead(Props.Target.Character))
	{
		return;
	}
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
	if (UAuraAbilitySystemLibrary::IsSuccessfulDebuff(Props.EffectContextHandle))
	{
		HandleDebuff(Props);
	}
}

void UAuraAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	if (Attribute == GetMaxHealthAttribute() && bTopOffHealth)
	{
		SetHealth(GetMaxHealth());
		bTopOffHealth = false;
	}
	if (Attribute == GetMaxManaAttribute() && bTopOffMana)
	{
		SetMana(GetMaxMana());
		bTopOffMana = false;
	}
}

void UAuraAttributeSet::FromSaveData(const UAuraSaveGame* SaveData)
{
}

void UAuraAttributeSet::ToSaveData(UAuraSaveGame* SaveData) const
{
	SaveData->Strength = GetStrength();
	SaveData->Intelligence = GetIntelligence();
	SaveData->Resilience = GetResilience();
	SaveData->Vigor = GetVigor();
}

void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.Source.AbilitySystemComponent = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	if (Props.Source.AbilitySystemComponent)
	{
		Props.Source.AvatarActor = Props.Source.AbilitySystemComponent->GetAvatarActor();
		Props.Source.Controller = Props.Source.AbilitySystemComponent->AbilityActorInfo->PlayerController.Get();
	}
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
	if (Data.Target.AbilityActorInfo)
	{
		Props.Target.AvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.Target.Controller = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.Target.Character = Cast<ACharacter>(Props.Target.AvatarActor);
		Props.Target.AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
			Props.Target.AvatarActor
		);
	}
}

void UAuraAttributeSet::HandleIncomingDamage(const FEffectProperties& Props)
{
	const float IncomingDamage = GetMeta_IncomingDamage();
	SetMeta_IncomingDamage(0.f);
	if (IncomingDamage > 0.f)
	{
		HandleOutgoingDamage(Props, IncomingDamage);
		const float NewHealth = GetHealth() - IncomingDamage;
		const bool bFatal = NewHealth <= 0.f;
		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
		if (!bFatal)
		{
			FGameplayTagContainer TagContainer;
			TagContainer.AddTag(
				ICombatInterface::GetHitReactAbilityTagByDamageType(
					Props.Target.AvatarActor,
					UAuraAbilitySystemLibrary::GetDamageTypeTag(Props.EffectContextHandle)
				)
			);
			Props.Target.AbilitySystemComponent->TryActivateAbilitiesByTag(
				TagContainer
			);
			if (const FVector KnockbackForce = UAuraAbilitySystemLibrary::GetKnockbackVector(Props.EffectContextHandle);
				!
				KnockbackForce.IsNearlyZero(1.f))
			{
				Props.Target.Character->LaunchCharacter(KnockbackForce, true, true);
			}
		}
		else
		{
			if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.Target.AvatarActor))
			{
				CombatInterface->Die();
				CombatInterface->ApplyDeathImpulse(
					UAuraAbilitySystemLibrary::GetDeathImpulse(Props.EffectContextHandle)
				);
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
	if (IPlayerInterface::ImplementsPlayerInterface(Props.Source.AvatarActor))
	{
		const int32 CurrentXP = IPlayerInterface::Execute_GetXP(Props.Source.Character);
		const int32 NewLevel = IPlayerInterface::Execute_FindLevelForXP(Props.Source.Character, CurrentXP + IncomingXP);
		const int32 CurrentLevel = ICombatInterface::GetCharacterLevel(Props.Source.Character);
		if (CurrentLevel < NewLevel)
		{
			for (int32 Idx = 0; Idx < NewLevel - CurrentLevel; Idx++)
			{
				// Get level up rewards (attribute & spell points)
				const FAuraLevelUpRewards& Rewards = IPlayerInterface::Execute_GetLevelUpRewards(
					Props.Source.Character,
					CurrentLevel + Idx
				);
				// Apply rewards
				IPlayerInterface::Execute_ApplyLevelUpRewards(
					Props.Source.Character,
					1,
					Rewards
				);
			}
			if (UAuraAbilitySystemComponent* AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(
				Props.Source.AbilitySystemComponent
			))
			{
				AuraAbilitySystemComponent->ServerUpdateAbilityStatuses(NewLevel);
			}
			// TODO - there are complications here that will be addressed later.
			bTopOffHealth = true;
			bTopOffHealth = true;
			SetHealth(GetMaxHealth());
			SetMana(GetMaxMana());
			IPlayerInterface::Execute_LevelUp(Props.Source.Character);
		}
		IPlayerInterface::Execute_AddToXP(Props.Source.Character, IncomingXP);
	}
}

void UAuraAttributeSet::HandleDebuff(const FEffectProperties& Props)
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	FGameplayEffectContextHandle EffectContext = Props.Source.AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(Props.Source.AvatarActor);

	const FGameplayTag DebuffTypeTag = UAuraAbilitySystemLibrary::GetDebuffTypeTag(Props.EffectContextHandle);
	const FGameplayTag DamageTypeTag = GameplayTags.GetDebuffTypeDamageTypeTag(DebuffTypeTag);
	const float DebuffDamage = UAuraAbilitySystemLibrary::GetDebuffDamage(Props.EffectContextHandle);
	const float DebuffDuration = UAuraAbilitySystemLibrary::GetDebuffDuration(Props.EffectContextHandle);
	const float DebuffFrequency = UAuraAbilitySystemLibrary::GetDebuffFrequency(Props.EffectContextHandle);

	const FString DebuffName = FString::Printf(TEXT("DynamicDebuff_%s"), *DebuffTypeTag.ToString());
	UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(DebuffName));

	Effect->DurationPolicy = EGameplayEffectDurationType::HasDuration;
	Effect->Period = DebuffFrequency;
	Effect->DurationMagnitude = FScalableFloat(DebuffDuration);

	Effect->StackingType = EGameplayEffectStackingType::AggregateBySource;
	Effect->StackLimitCount = 1;

	const int32 Index = Effect->Modifiers.Num();
	Effect->Modifiers.Add(FGameplayModifierInfo());
	FGameplayModifierInfo& ModifierInfo = Effect->Modifiers[Index];

	ModifierInfo.ModifierMagnitude = FScalableFloat(DebuffDamage);
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.Attribute = GetMeta_IncomingDamageAttribute();

	UTargetTagsGameplayEffectComponent& TagsComponent = Effect->FindOrAddComponent<
		UTargetTagsGameplayEffectComponent>();
	FInheritedTagContainer InheritedTagContainer = FInheritedTagContainer();
	InheritedTagContainer.Added.AddTag(DebuffTypeTag);
	if (DebuffTypeTag.MatchesTagExact(GameplayTags.Debuff_Type_Shock))
	{
		InheritedTagContainer.Added.AddTag(FAuraGameplayTags::Get().Player_Block_CursorTrace);
		InheritedTagContainer.Added.AddTag(FAuraGameplayTags::Get().Player_Block_Interaction);
		InheritedTagContainer.Added.AddTag(FAuraGameplayTags::Get().Player_Block_Movement);
		InheritedTagContainer.Added.AddTag(FAuraGameplayTags::Get().Player_Block_Ability_Offensive);
	}
	TagsComponent.SetAndApplyTargetTagChanges(InheritedTagContainer);

	if (const FGameplayEffectSpec* MutableSpec = new FGameplayEffectSpec(Effect, EffectContext, 1.f))
	{
		FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(MutableSpec->GetContext().
			Get());
		const TSharedPtr<FGameplayTag> DebuffDamageType = MakeShareable(new FGameplayTag(DamageTypeTag));
		AuraContext->SetDamageTypeTag(DebuffDamageType);

		Props.Target.AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*MutableSpec);
	}
}

void UAuraAttributeSet::HandleOutgoingDamage(const FEffectProperties& Props, const float IncomingDamage)
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	if (UAuraAbilitySystemComponent* AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(
		Props.Source.AbilitySystemComponent
	))
	{
		AuraAbilitySystemComponent->OnOutgoingDamageDelegate.Broadcast(
			FOnAbilitySystemOutgoingDamagePayload(Props.Source.AvatarActor, Props.Target.AvatarActor, IncomingDamage)
		);
	}
}


void UAuraAttributeSet::ShowDamageText(const FEffectProperties& Props, const float& IncomingDamage) const
{
	if (!Props.Source.Character || !Props.Target.Character)
	{
		return;
	}
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
		else if (AAuraPlayerController* TargetPlayerController = Cast<AAuraPlayerController>(
			Props.Target.Character->GetController()
		))
		{
			// show damage on player
			TargetPlayerController->ShowDamageNumber(
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
	const int32 XPReward = ICombatInterface::GetXPReward(Props.Target.AvatarActor);
	FGameplayEventData Payload;
	Payload.EventTag = FAuraGameplayTags::Get().Attributes_Meta_IncomingXP;
	Payload.EventMagnitude = XPReward;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		Props.Source.AvatarActor,
		Payload.EventTag,
		Payload
	);
}
