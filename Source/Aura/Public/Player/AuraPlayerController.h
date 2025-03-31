// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interaction/HighlightInterface.h"
#include "AuraPlayerController.generated.h"

class AMagicCircle;
class UNiagaraSystem;
class UDamageTextComponent;
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
		if (IsDifferentPtr())
		{
			UnHighlightLast();
			HighlightCurrent();
		}
	}

	void Clear()
	{
		UnHighlightCurrent();
		UnHighlightLast();
		CurrentActor = nullptr;
		LastActor = nullptr;
	}

	bool HasCurrentTarget() const
	{
		return CurrentActor != nullptr;
	}

	bool IsDifferentPtr() const
	{
		return LastActor != CurrentActor;
	}

	void HighlightCurrent() const
	{
		if (CurrentActor != nullptr)
		{
			CurrentActor->HighlightActor();
		}
	}

	void UnHighlightCurrent() const
	{
		if (CurrentActor != nullptr)
		{
			CurrentActor->UnHighlightActor();
		}
	}

	void UnHighlightLast() const
	{
		if (LastActor != nullptr)
		{
			LastActor->UnHighlightActor();
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

	UFUNCTION(Client, Reliable)
	virtual void ShowDamageNumber(
		AActor* Target,
		float DamageAmount,
		const bool IsBlockedHit,
		const bool IsCriticalHit
	);

	UFUNCTION(BlueprintCallable)
	void ShowMagicCircle(UMaterialInterface* DecalMaterial = nullptr);
	UFUNCTION(BlueprintCallable)
	void HideMagicCircle();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditAnywhere, Category = "Input", meta=(DisplayName="Attack Target"))
	TObjectPtr<UInputAction> Input_AttackTarget;

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

	// UI
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;

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
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	TObjectPtr<UNiagaraSystem> ClickNiagaraSystem;
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	TSubclassOf<AMagicCircle> MagicCircleClass;
	UPROPERTY(VisibleInstanceOnly, Category = "Combat")
	TObjectPtr<AMagicCircle> MagicCircle;
	FHitResult CursorHit;

	void AutoMove_Start();
	void AutoMove_Process();
	void AutoMove_End();
	void UpdateMagicCircleLocation();
};
