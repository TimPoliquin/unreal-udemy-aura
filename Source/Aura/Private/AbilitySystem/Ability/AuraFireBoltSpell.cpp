// Copyright Alien Shores


#include "AbilitySystem/Ability/AuraFireBoltSpell.h"

#include "Tags/AuraGameplayTags.h"

FString UAuraFireBoltSpell::GetDescription(const int32 AbilityLevel) const
{
	const float ManaCost = GetManaCost(AbilityLevel);
	const float Cooldown = GetCooldown(AbilityLevel);
	const int32 Damage = GetDamageAtLevel(AbilityLevel);
	const FString Bolts = AbilityLevel == 1
		                      ? FString::Printf(TEXT("a bolt"))
		                      : FString::Printf(
			                      TEXT("%d bolts"),
			                      FMath::Min(AbilityLevel, NumProjectiles)
		                      );
	return FString::Printf(
		TEXT(
			// Title
			"" RICH_TITLE("%s") "\n\n"
			// Details
			RICH_SMALL("Level: ") RICH_LEVEL("%d") "\n" // Level
			RICH_SMALL("Mana Cost: ") RICH_MANA_COST("%.1f") "\n" // Mana Cost
			RICH_SMALL("Cooldown: ") RICH_COOLDOWN("%.1f") "\n\n" // Cooldown
			// Description
			RICH_DEFAULT("Launches %s of fire, exploding on impact and dealing ") RICH_DAMAGE("%d")
			RICH_DEFAULT(
				"fire damage with a chance to burn."
			) "\n\n"
		),
		*GetAbilityName(),
		AbilityLevel,
		FMath::Abs(ManaCost),
		FMath::Abs(Cooldown),
		*Bolts,
		Damage
	);
}
