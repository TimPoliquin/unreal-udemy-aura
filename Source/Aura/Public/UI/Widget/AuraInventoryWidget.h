// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AuraInventoryWidget.generated.h"

class UAuraInventoryItemWidget;
class UUniformGridPanel;
/**
 * 
 */
UCLASS()
class AURA_API UAuraInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	TArray<UAuraInventoryItemWidget*> CreateInventoryItems(UUniformGridPanel* GridPanel, int32 Columns, int32 Rows);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> InventoryItemWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 RowCount = 5;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ColumnCount = 5;
};
