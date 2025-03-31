// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraDamageGameplayAbility.h"
#include "AuraFireBlast.generated.h"

class AAuraFireball;
/**
 * 
 */
UCLASS()
class AURA_API UAuraFireBlast : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

public:
	virtual FString GetDescription(const int32 AbilityLevel) const override;
	UFUNCTION(BlueprintCallable, Category = "FireBlast")
	TArray<AAuraFireball*> SpawnFireballs();

protected:
	UPROPERTY(EditDefaultsOnly, Category="FireBlast")
	int32 NumFireballs = 12;

private:
	UPROPERTY(EditDefaultsOnly, Category = "FireBlast")
	TSubclassOf<AAuraFireball> FireballClass;
};
