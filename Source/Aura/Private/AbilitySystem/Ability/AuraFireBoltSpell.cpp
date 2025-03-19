// Copyright Alien Shores


#include "AbilitySystem/Ability/AuraFireBoltSpell.h"

#include "Tags/AuraGameplayTags.h"

FString UAuraFireBoltSpell::GetDescription(const int32 AbilityLevel) const
{
	const int32 Damage = GetDamageByTypeAtLevel(FAuraGameplayTags::Get().Damage_Fire, AbilityLevel);
	const FString Title = URichTextUtils::Title(GetAbilityName());
	const FString Footer = URichTextUtils::Small("Level: ") + URichTextUtils::Level(AbilityLevel);
	const FString DamageText = URichTextUtils::Damage(Damage) + URichTextUtils::Default(
		" fire damage with a chance to burn"
	);
	const FString Bolts = AbilityLevel == 1
		                      ? URichTextUtils::Default(
			                      TEXT("Launches a bolt of fire, exploding on impact and dealing:")
		                      )
		                      : URichTextUtils::Default(
			                      FString::Printf(
				                      TEXT("Launches %d bolts of fire, exploding on impact and dealing:"),
				                      FMath::Min(AbilityLevel, NumProjectiles)
			                      )
		                      );
	return FString::Printf(TEXT("%s\n\n%s %s\n\n%s"), *Title, *Bolts, *DamageText, *Footer);
}
