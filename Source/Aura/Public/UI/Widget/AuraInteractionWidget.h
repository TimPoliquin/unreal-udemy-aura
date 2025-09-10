// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraWidgetVisibilityEvents.h"
#include "InteractionWidgetInterface.h"
#include "Blueprint/UserWidget.h"
#include "AuraInteractionWidget.generated.h"


class UTextBlock;
/**
 * 
 */
UCLASS()
class AURA_API UAuraInteractionWidget : public UUserWidget, public IInteractionWidgetInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="Event")
	FAuraWidgetEventSignature OnShown;
	UPROPERTY(BlueprintAssignable, Category="Event")
	FAuraWidgetEventSignature OnHidden;

	/** Start IInteractionWidgetInterface **/
	virtual void SetInteractionKey_Implementation(const FString& Key) override;
	virtual void SetActionText_Implementation(const FString& Key) override;
	virtual void SetStartHidden_Implementation(const bool bInStartHidden) override;
	virtual void SetAutoDestroyOnHide_Implementation(const bool bInAutoDestroyOnHide) override;
	virtual void Show_Implementation() override;
	virtual void Hide_Implementation() override;
	/** End IInteractionWidgetInterface **/


protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent)
	void InitializeActionText();
	UFUNCTION(BlueprintImplementableEvent)
	void InitializeKeyText();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void InitializeHiddenState();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void InitializeShownState();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Shown();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Hidden();

	UFUNCTION(BlueprintCallable)
	static void SetDisplayText(FString DisplayText, UTextBlock* TextWidget);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Properties")
	bool bStartHidden = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Properties")
	bool bAutoDestroyOnHide = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Properties")
	FString ActionText = TEXT("Interact");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Properties")
	FString KeyText = EKeys::E.GetDisplayName().ToString();
};
