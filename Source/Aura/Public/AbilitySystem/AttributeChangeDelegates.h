#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AuraAbilitySystemTypes.h"
#include "AttributeChangeDelegates.generated.h"

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangedSignature, int32, NewValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStatInitializedSignature, int32, NewValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnPlayerAbilityStatusChangedSignature,
	const int32,
	PlayerLevel,
	const TArray<FAbilityTagStatus>&,
	AbilityStatuses
);
