// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AuraLevelTransition.generated.h"

struct FAuraLevelTransitionParams;
DECLARE_MULTICAST_DELEGATE_OneParam(FAuraLevelTransitionCompleteSignature, UWorld* World);

/**
 * 
 */
UCLASS()
class AURA_API UAuraLevelTransition : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(const FAuraLevelTransitionParams& Params);
	FName GetDestinationPlayerStartTag() const;
	FAuraLevelTransitionCompleteSignature OnComplete;

private:
	FString SaveSlot = FString("");
	int32 SlotIndex = 0;
	FName DestinationPlayerStartTag = NAME_None;
	bool bShouldLoad = false;
};
