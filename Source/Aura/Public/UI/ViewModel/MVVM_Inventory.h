// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_Inventory.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMVVM_Inventory : public UMVVMViewModelBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess="true"))
	FString InventoryName = FString("Inventory");

public:
	FString GetInventoryName() const;
	void SetInventoryName(const FString& InInventoryName);
};
