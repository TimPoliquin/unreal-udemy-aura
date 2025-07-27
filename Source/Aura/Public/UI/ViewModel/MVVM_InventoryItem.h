// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MVVMViewModelBase.h"
#include "UObject/Object.h"
#include "MVVM_InventoryItem.generated.h"

class AAuraCharacter;
/**
 * 
 */
UCLASS()
class AURA_API UMVVM_InventoryItem : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void InitializeDependencies(AAuraCharacter* PlayerActor);
	FGameplayTag GetInventoryItemTag() const;
	void SetInventoryItemTag(const FGameplayTag& InInventoryItemTag);
	int32 GetQuantity() const;
	void SetQuantity(const int32 InQuantity);
	int32 GetRow() const;
	void SetRow(const int32 InRow);
	int32 GetColumn() const;
	void SetColumn(const int32 InColumn);

protected:
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	FGameplayTag InventoryItemTag = FGameplayTag::EmptyTag;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	int32 Quantity = 0;
	UPROPERTY(BlueprintReadOnly)
	int32 Row = -1;
	UPROPERTY(BlueprintReadOnly)
	int32 Column = -1;
};
