#pragma once
#include "AuraEquipmentTypes.h"
#include "GameplayTagContainer.h"

#include "AuraEquipmentEvents.generated.h"

USTRUCT(BlueprintType)
struct FAuraEquipmentDelegatePayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	EAuraEquipmentSlot EquipmentSlot = EAuraEquipmentSlot::None;
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag EquipmentTag;
};

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAuraEquipmentDelegateSignature, const FAuraEquipmentDelegatePayload&, Payload);
