// Copyright Alien Shores


#include "AbilitySystem/Ability/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"
#include "Tags/AuraGameplayTags.h"
#include "Utils/ArrayUtils.h"

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

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor) const
{
	FDamageEffectParams DamageEffectParams;
	DamageEffectParams.FillFromDamageConfig(DamageConfig);
	DamageEffectParams.WorldContextObject = GetAvatarActorFromActorInfo();
	DamageEffectParams.DamageGameplayEffectClass = DamageEffectClass;
	DamageEffectParams.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	DamageEffectParams.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
		TargetActor
	);
	DamageEffectParams.BaseDamage = GetDamageAtLevel(GetAbilityLevel());
	DamageEffectParams.AbilityLevel = GetAbilityLevel();
	if (IsValid(TargetActor))
	{
		FRotator Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).
			Rotation();
		Rotation.Pitch = 25.f;
		DamageEffectParams.DeathImpulse = Rotation.Vector() * DamageEffectParams.DeathImpulseMagnitude;
		DamageEffectParams.KnockbackForce = Rotation.Vector() * DamageEffectParams.KnockbackForceMagnitude;
	}
	return DamageEffectParams;
}

int32 UAuraDamageGameplayAbility::GetDamageAtLevel(
	const int32 AbilityLevel
) const
{
	return DamageConfig.Amount.GetValueAtLevel(AbilityLevel);
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
