// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraProjectileSpell.h"
#include "AuraFireBoltSpell.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraFireBoltSpell : public UAuraProjectileSpell
{
	GENERATED_BODY()

	virtual FString GetDescription(const int32 AbilityLevel) const override;
};
