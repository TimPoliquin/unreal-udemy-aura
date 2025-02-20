// Copyright Alien Shores


#include "AbilitySystem/Data/CharacterClassInfo.h"

FCharacterClassDefaultInfo UCharacterClassInfo::GetClassDefaultInfo(const ECharacterClass CharacterClass) const
{
	return CharacterClassInformation.FindChecked(CharacterClass);
}

float UCharacterClassInfo::GetArmorPenetrationCoefficient(const int Level) const
{
	return DamageCalculationCoefficients->FindCurve(
		FName("ArmorPenetration"),
		FString()
	)->Eval(Level);
}

float UCharacterClassInfo::GetEffectiveArmorCoefficient(const int Level) const
{
	return DamageCalculationCoefficients->FindCurve(
		FName("EffectiveArmor"),
		FString()
	)->Eval(Level);
}

float UCharacterClassInfo::GetCriticalHitResistanceCoefficient(const int Level) const
{
	return DamageCalculationCoefficients->FindCurve(
		FName("CriticalHitResistance"),
		FString()
	)->Eval(Level);
}
