// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraBeamSpell.h"
#include "AuraElectrocuteSpell.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraElectrocuteSpell : public UAuraBeamSpell
{
	GENERATED_BODY()

public:
	virtual FString GetDescription_Implementation(const int32 AbilityLevel) const override;
};
