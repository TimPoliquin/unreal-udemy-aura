// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AuraLevelTransition.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FAuraLevelTransitionCompleteSignature, UWorld* World);

/**
 * 
 */
UCLASS()
class AURA_API UAuraLevelTransition : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(const FName& InDestinationPlayerStartTag);
	FName GetDestinationPlayerStartTag() const;
	FAuraLevelTransitionCompleteSignature OnComplete;

private:
	FName DestinationPlayerStartTag = NAME_None;
};
