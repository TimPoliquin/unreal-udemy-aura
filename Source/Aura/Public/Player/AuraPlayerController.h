// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interaction/HighlightInterface.h"
#include "AuraPlayerController.generated.h"

class USplineComponent;
class UAuraAbilitySystemComponent;
struct FGameplayTag;
class UAuraInputConfiguration;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;

struct FHighlightContext
{
	FHighlightContext()
	{
	}

	TScriptInterface<IHighlightInterface> LastActor;
	TScriptInterface<IHighlightInterface> CurrentActor;

	void Track(AActor* Actor)
	{
		LastActor = CurrentActor;
		CurrentActor = Actor;
		if (IsBothNullPtr())
		{
			// both LastActor and ThisActor are null, do nothing
		}
		else if (IsLastNullPtr())
		{
			// LastActor is null and ThisActor is not, so highlight ThisActor
			HighlightCurrent();
		}
		else if (IsCurrentNullPtr())
		{
			// LastActor is valid and ThisActor is null, so unhighlight LastActor
			UnHighlightLast();
		}
		else if (IsDifferentPtr())
		{
			// Both are valid, but are not the same, so unhighlight LastActor and Highlight ThisActor
			UnHighlightLast();
			HighlightCurrent();
		}
	}

	bool IsBothNullPtr() const
	{
		return LastActor == nullptr && CurrentActor == nullptr;
	}

	bool IsLastNullPtr() const
	{
		return LastActor == nullptr;
	}

	bool HasCurrentTarget() const
	{
		return CurrentActor != nullptr;
	}

	bool IsCurrentNullPtr() const
	{
		return CurrentActor == nullptr;
	}

	bool IsBothSamePtr() const
	{
		return LastActor == CurrentActor;
	}

	bool IsDifferentPtr() const
	{
		return LastActor != CurrentActor;
	}

	void HighlightLast() const
	{
		if (LastActor != nullptr)
		{
			LastActor->HighlightActor();
		}
	}

	void HighlightCurrent() const
	{
		if (CurrentActor != nullptr)
		{
			CurrentActor->HighlightActor();
		}
	}

	void UnHighlightLast() const
	{
		if (LastActor != nullptr)
		{
			LastActor->UnHighlightActor();
		}
	}

	void UnHighlightCurrent() const
	{
		if (CurrentActor != nullptr)
		{
			CurrentActor->UnHighlightActor();
		}
	}
};

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAuraPlayerController();
	virtual void PlayerTick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	FHighlightContext HighlightContext;

	void Move(const FInputActionValue& Value);
	void CursorTrace();

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UAuraInputConfiguration> InputConfig;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;
	UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent();

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	// Character Movement / Targeting
	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float ShortPressThreshold = 0.5f;
	bool bAutoRunning = false;
	bool bTargeting = false;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float AutoRunAcceptanceRadius = 50.f;
	UPROPERTY(VisibleAnywhere, Category = "Movement")
	TObjectPtr<USplineComponent> Spline;

	void AutoMove_Start();
	void AutoMove_Process();
	void AutoMove_End();
};
