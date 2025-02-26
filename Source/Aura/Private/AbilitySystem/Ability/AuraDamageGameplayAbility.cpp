// Copyright Alien Shores


#include "AbilitySystem/Ability/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"
#include "Tags/AuraGameplayTags.h"
#include "Utils/ArrayUtils.h"

void UAuraDamageGameplayAbility::DealDamage(AActor* TargetActor)
{
	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1);
	const int32 Level = GetAbilityLevel();
	for (const FGameplayTag& DamageTag : FAuraGameplayTags::Get().GetDamageTypes())
	{
		float DamageMagnitude = 0.f;
		if (DamageTypes.Contains(DamageTag))
		{
			DamageMagnitude = DamageTypes[DamageTag].GetValueAtLevel(Level);
		}
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageTag, DamageMagnitude);
	}
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
