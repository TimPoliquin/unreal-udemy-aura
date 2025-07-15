// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraPOI.h"
#include "Interaction/HighlightInterface.h"
#include "AuraPOILock.generated.h"

class IAuraGateInterface;
class UUserWidget;

UCLASS()
class AURA_API AAuraPOILock : public AAuraPOI, public IHighlightInterface
{
	GENERATED_BODY()

public:
	AAuraPOILock();

	/** IHighlightInterface Start **/
	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;
	/** IHighlightInterface End **/
protected:
	virtual void BeginPlay() override;
	virtual bool IsPreconditionMet_Implementation(AActor* Player) const override;
	UFUNCTION(BlueprintImplementableEvent)
	void InitializeInteractionWidgetSettings(UUserWidget* InInteractionWidget, const FString& InInteractionText);
	UFUNCTION(BlueprintImplementableEvent)
	void InitializePreconditionWidgetSettings(UUserWidget* InPreconditionWidget, const FSlateColor InRuneColor, const UTexture2D* InRuneIcon);
	virtual bool HandleInteract_Implementation(AActor* Player) override;
	UFUNCTION(BlueprintCallable)
	bool Unlock(AActor* Player);
	UFUNCTION(BlueprintImplementableEvent)
	void PlayUnlockEffect(AActor* Player);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> LockMeshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Lock", meta=(Categories="Item.Type.Key"))
	FGameplayTag KeyTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Lock", meta=(AllowedClasses="/Script/Aura.AuraGateInterface"))
	TObjectPtr<AActor> Gate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Lock")
	FSlateColor RuneColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Lock")
	TObjectPtr<UTexture2D> RuneIcon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Lock")
	TObjectPtr<USoundBase> UnlockSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Lock")
	FString InteractText = FString("Unlock");
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Lock")
	bool bUnlocked = false;
};
