// Copyright Alien Shores


#include "UI/Widget/AuraInventoryWidget.h"

#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "UI/Widget/AuraInventoryItemWidget.h"

TArray<UAuraInventoryItemWidget*> UAuraInventoryWidget::CreateInventoryItems(
	UUniformGridPanel* GridPanel,
	int32 Columns,
	int32 Rows
)
{
	TArray<UAuraInventoryItemWidget*> InventoryItems;
	for (int32 Row = 0; Row < Rows; Row++)
	{
		for (int32 Column = 0; Column < Columns; Column++)
		{
			UAuraInventoryItemWidget* InventoryItem =
				CreateWidget<UAuraInventoryItemWidget>(
					this,
					InventoryItemWidgetClass,
					FName(*FString::Printf(TEXT("Inventory Item %d - %d"), Row, Column))
				);
			InventoryItems.Add(InventoryItem);
			if (UUniformGridSlot* Slot = GridPanel->AddChildToUniformGrid(InventoryItem))
			{
				Slot->SetRow(Row);
				Slot->SetColumn(Column);
				Slot->SetHorizontalAlignment(HAlign_Center);
				Slot->SetVerticalAlignment(VAlign_Center);
			}
		}
	}
	return InventoryItems;
}
