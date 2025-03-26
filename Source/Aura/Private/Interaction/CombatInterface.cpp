// Copyright Alien Shores


#include "Interaction/CombatInterface.h"

#include "Utils/ArrayUtils.h"


int32 ICombatInterface::GetCharacterLevel(const UObject* Actor)
{
	if (Actor && Actor->Implements<UCombatInterface>())
	{
		return Execute_GetCharacterLevel(Actor);
	}
	return 0;
}

bool ICombatInterface::IsAlive(const UObject* Actor)
{
	if (Actor && Actor->Implements<UCombatInterface>())
	{
		return !Execute_IsDead(Actor);
	}
	return false;
}


bool ICombatInterface::IsDead(const UObject* Actor)
{
	if (Actor && Actor->Implements<UCombatInterface>())
	{
		return Execute_IsDead(Actor);
	}
	return false;
}

TArray<FTaggedMontage> ICombatInterface::GetAttackMontages(const UObject* Actor)
{
	if (Actor && Actor->Implements<UCombatInterface>())
	{
		return Execute_GetAttackMontages(Actor);
	}
	return TArray<FTaggedMontage>();
}

TArray<FName> ICombatInterface::GetTargetTagsToIgnore(const UObject* Actor)
{
	if (Actor && Actor->Implements<UCombatInterface>())
	{
		return Execute_GetTargetTagsToIgnore(Actor);
	}
	return TArray<FName>();
}

int32 ICombatInterface::GetXPReward(const UObject* Actor)
{
	if (Actor && Actor->Implements<UCombatInterface>())
	{
		return Execute_GetXPReward(Actor);
	}
	return 0;
}

void ICombatInterface::SetActiveAbilityTag(UObject* Actor, const FGameplayTag& ActiveAbilityTag)
{
	if (Actor && Actor->Implements<UCombatInterface>())
	{
		Execute_SetActiveAbilityTag(Actor, ActiveAbilityTag);
	}
}

void ICombatInterface::ClearActiveAbilityTag(UObject* Actor)
{
	if (Actor && Actor->Implements<UCombatInterface>())
	{
		Execute_ClearActiveAbilityTag(Actor);
	}
}

void ICombatInterface::UpdateFacingTarget(UObject* Actor, const FVector& FacingTarget)
{
	if (Actor && Actor->Implements<UCombatInterface>())
	{
		Execute_UpdateFacingTarget(Actor, FacingTarget);
	}
}

USkeletalMeshComponent* ICombatInterface::GetWeapon(const UObject* Actor)
{
	if (Actor && Actor->Implements<UCombatInterface>())
	{
		return Execute_GetWeapon(Actor);
	}
	return nullptr;
}
