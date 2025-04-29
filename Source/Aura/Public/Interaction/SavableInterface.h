// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SavableInterface.generated.h"

class UAuraSaveGame;
// This class does not need to be modified.
UINTERFACE()
class USavableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API ISavableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void FromSaveData(const UAuraSaveGame* SaveData) = 0;
	virtual void ToSaveData(UAuraSaveGame* SaveData) const = 0;
};
