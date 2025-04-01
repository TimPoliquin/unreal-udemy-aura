// Copyright Alien Shores


#include "AbilitySystem/Ability/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"
#include "Tags/AuraGameplayTags.h"
#include "Utils/ArrayUtils.h"

void UAuraDamageGameplayAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (bAutoActivateAbilityTag)
	{
		ICombatInterface::SetActiveAbilityTag(GetAvatarActorFromActorInfo(), GetDefaultAbilityTag());
	}
}

void UAuraDamageGameplayAbility::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	if (bAutoDeactivateAbilityTag)
	{
		ICombatInterface::ClearActiveAbilityTag(GetAvatarActorFromActorInfo());
	}
}


void UAuraDamageGameplayAbility::DealDamage(AActor* TargetActor)
{
	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1);
	const int32 Level = GetAbilityLevel();
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		DamageConfig.DamageTypeTag,
		DamageConfig.Amount.GetValueAtLevel(Level)
	);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(
		*SpecHandle.Data.Get(),
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor)
	);
}

FTaggedMontage UAuraDamageGameplayAbility::GetRandomAttackMontage() const
{
	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (const TArray<FTaggedMontage> AttackMontages = ICombatInterface::GetAttackMontages(AvatarActor); AttackMontages.
		Num() > 0)
	{
		return UArrayUtils::GetRandomElement(AttackMontages);
	}
	UE_LOG(LogTemp, Warning, TEXT("No attack montage tags associated for %s"), *AvatarActor->GetName());
	return FTaggedMontage();
}

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(
	AActor* TargetActor,
	FVector RadialDamageOrigin,
	bool bOverrideKnockbackDirection,
	FVector InKnockbackDirectionOverride,
	bool bOverrideDeathImpulse,
	FVector InDeathImpulseDirectionOverride,
	bool bOverridePitch,
	float PitchOverride
) const
{
	return UAuraAbilitySystemLibrary::MakeCustomDamageEffectParams(
		GetAvatarActorFromActorInfo(),
		TargetActor,
		DamageEffectClass,
		DamageConfig,
		GetAbilityLevel(),
		RadialDamageOrigin,
		bOverrideKnockbackDirection,
		InKnockbackDirectionOverride,
		bOverrideDeathImpulse,
		InDeathImpulseDirectionOverride,
		bOverridePitch,
		PitchOverride
	);
}

int32 UAuraDamageGameplayAbility::GetDamageAtLevel(
	const int32 AbilityLevel
) const
{
	return DamageConfig.GetDamageAtLevel(AbilityLevel);
}

float UAuraDamageGameplayAbility::GetManaCost(const float InLevel) const
{
	if (GetCostGameplayEffect())
	{
		for (const FGameplayModifierInfo Mod : GetCostGameplayEffect()->Modifiers)
		{
			if (Mod.Attribute == UAuraAttributeSet::GetManaAttribute())
			{
				float ManaCost = 0.f;
				// TODO this assumes a static value.
				Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, ManaCost);
				return ManaCost;
			}
		}
	}
	return 0.f;
}

float UAuraDamageGameplayAbility::GetCooldown(const float InLevel) const
{
	if (const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
	{
		float Cooldown = 0.f;
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);
		return Cooldown;
	}
	return 0.f;
}

FGameplayTag UAuraDamageGameplayAbility::GetDefaultAbilityTag() const
{
	return GetAssetTags().First();
}
