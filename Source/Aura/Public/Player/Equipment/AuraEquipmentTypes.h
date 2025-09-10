#pragma once

#include "AuraEquipmentTypes.generated.h"

UENUM(BlueprintType)
enum class EAuraEquipmentSlot : uint8
{
	/** None */
	None,
	/** Tool slot */
	Tool,
	/** Weapon slot */
	Weapon
};

UENUM(BlueprintType)
enum class EAuraEquipmentUseMode : uint8
{
	/** The player is using nothing */
	None,
	/** The player is using a tool */
	Tool,
	/** The player is using a weapon */
	Weapon
};
