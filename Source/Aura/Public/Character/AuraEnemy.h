// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraBaseCharacter.h"
#include "Interaction/HighlightInterface.h"
#include "AbilitySystem/AttributeChangeDelegates.h"
#include "AuraEnemy.generated.h"

class UWidgetComponent;

UCLASS()
class AURA_API AAuraEnemy : public AAuraBaseCharacter, public IHighlightInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();
	void InitializeAttributeDelegates();
	virtual void Tick(float DeltaTime) override;

	// IHighlightInterface
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	// ICombatInterface
	FORCEINLINE virtual int GetCharacterLevel() const override
	{
		return Level;
	}

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;

protected:
	virtual void BeginPlay() override;
	virtual void InitializeAbilityActorInfo() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Highlight")
	bool bHighlighted;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Default Attributes")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> HealthWidget;

private:
	UPROPERTY(EditAnywhere, Category = "Highlight")
	uint8 HighlightCustomDepthStencilValue = 250;
};
