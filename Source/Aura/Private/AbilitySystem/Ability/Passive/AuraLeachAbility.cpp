// Copyright Alien Shores


#include "AbilitySystem/Ability/Passive/AuraLeachAbility.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Tags/AuraGameplayTags.h"

FString UAuraLeachAbility::GetDescription_Implementation(const int32 AbilityLevel) const
{
	const float ManaCost = GetManaCost(AbilityLevel);
	const float Cooldown = GetCooldown(AbilityLevel);
	const float LeachPercentage = GetLeachAmountAtLevel(AbilityLevel) * 100.f;
	return FString::Printf(
		TEXT(
			// Title
			"" RICH_TITLE("%s") "\n\n"
			// Details
			RICH_SMALL("Level: ") RICH_LEVEL("%d") "\n" // Level
			RICH_SMALL("Mana Cost: ") RICH_MANA_COST("%.1f") "\n" // Mana Cost
			RICH_SMALL("Cooldown: ") RICH_COOLDOWN("%.1f") "\n\n" // Cooldown
			// Description
			RICH_DEFAULT("Absorbs ") RICH_BUFF("%d%%") RICH_DEFAULT(" of damage dealt as regenerated %s.")
			"\n\n"
		),
		*GetAbilityName(),
		AbilityLevel,
		FMath::Abs(ManaCost),
		FMath::Abs(Cooldown),
		FMath::RoundToInt(LeachPercentage),
		*LeachAttribute
	);
}

float UAuraLeachAbility::GetLeachMagnitude(const FOnAbilitySystemOutgoingDamagePayload& Payload) const
{
	const float OutgoingDamage = Payload.OutgoingDamage;
	const float LeachAmount = GetLeachAmountAtLevel(GetAbilityLevel());
	return OutgoingDamage * LeachAmount;
}

FActiveGameplayEffectHandle UAuraLeachAbility::ApplyLeachEffect(
	const FOnAbilitySystemOutgoingDamagePayload& Payload
)
{
	return UAuraAbilitySystemLibrary::ApplyBasicGameplayEffectWithMagnitude(
		GetAvatarActorFromActorInfo(),
		LeachEffect,
		GetAbilityLevel(),
		FAuraGameplayTags::Get().Effect_Magnitude,
		GetLeachMagnitude(Payload)
	);
}

float UAuraLeachAbility::GetLeachAmountAtLevel(const int32 Level) const
{
	if (LeachConfig.IsValid())
	{
		return LeachConfig.GetValueAtLevel(Level);
	}
	return -999;
}
