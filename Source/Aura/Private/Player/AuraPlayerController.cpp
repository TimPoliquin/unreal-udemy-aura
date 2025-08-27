// Copyright Alien Shores


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Actor/MagicCircle.h"
#include "Aura/Aura.h"
#include "CommonInputSubsystem.h"
#include "Aura/AuraLogChannels.h"
#include "Camera/CameraComponent.h"
#include "Character/EnemyInterface.h"
#include "Input/AuraInputComponent.h"
#include "Tags/AuraGameplayTags.h"
#include "UI/Widget/DamageTextComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	// Stop the game if AuraContext is not set
	check(AuraContext);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer()
	))
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}
	if (UCommonInputSubsystem* CommonSubsystem = UCommonInputSubsystem::Get(GetLocalPlayer()))
	{
		CommonSubsystem->OnInputMethodChangedNative.AddUObject(this, &AAuraPlayerController::InitializeInputMode);
		InitializeInputMode(CommonSubsystem->GetCurrentInputType());
	}
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
	UpdateMagicCircleLocation();
}

void AAuraPlayerController::ShowMagicCircle(UMaterialInterface* DecalMaterial)
{
	if (!IsValid(MagicCircle))
	{
		MagicCircle = GetWorld()->SpawnActor<AMagicCircle>(MagicCircleClass);
		MagicCircle->SetDecalMaterial(DecalMaterial);
		SetShowMouseCursor(false);
	}
}

void AAuraPlayerController::HideMagicCircle()
{
	if (IsValid(MagicCircle))
	{
		MagicCircle->Destroy();
		MagicCircle = nullptr;
		SetShowMouseCursor(true);
	}
}

void AAuraPlayerController::ShowDamageNumber_Implementation(
	AActor* Target,
	const float DamageAmount,
	const bool IsBlockedHit,
	const bool IsCriticalHit
)
{
	if (IsValid(Target) && DamageTextComponentClass && IsLocalController())
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(Target, DamageTextComponentClass);
		DamageText->RegisterComponent();
		DamageText->AttachToComponent(Target->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(DamageAmount, IsBlockedHit, IsCriticalHit);
	}
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAbilityActions(
		InputConfig,
		this,
		&AAuraPlayerController::AbilityInputTagPressed,
		&AAuraPlayerController::AbilityInputTagReleased,
		&AAuraPlayerController::AbilityInputTagHeld
	);
	AuraInputComponent->BindAction(
		MoveAction,
		ETriggerEvent::Triggered,
		this,
		&AAuraPlayerController::Move
	);
}

void AAuraPlayerController::Move(const FInputActionValue& Value)
{
	if (GetAuraAbilitySystemComponent() && GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(
		FAuraGameplayTags::Get().Player_Block_Movement
	))
	{
		return;
	}
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		const FVector2D InputAxisVector = Value.Get<FVector2D>();
		// const FRotator Rotation = GetControlRotation();
		const FRotator Rotation = ControlledPawn->FindComponentByClass<UCameraComponent>()->
		                                          GetComponentRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::CursorTrace()
{
	if (GetAuraAbilitySystemComponent() && GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(
		FAuraGameplayTags::Get().Player_Block_CursorTrace
	))
	{
		HighlightContext.Clear();
		return;
	}
	const ECollisionChannel TraceChannel = IsValid(MagicCircle)
		                                       ? ECC_ExcludeCharacters
		                                       : ECC_Target;
	GetHitResultUnderCursor(TraceChannel, false, CursorHit);
	if (CursorHit.bBlockingHit)
	{
		HighlightContext.Track(CursorHit.GetActor());
		if (HighlightContext.HasCurrentTarget())
		{
			TargetingStatus = HighlightContext.HasCurrentTarget() && IEnemyInterface::IsEnemyActor(
				                  HighlightContext.CurrentActor
			                  )
				                  ? ETargetingStatus::TargetingEnemy
				                  : ETargetingStatus::TargetingOther;
		}
		else
		{
			TargetingStatus = ETargetingStatus::NotTargeting;
		}
	}
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetAuraAbilitySystemComponent()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>())
		);
	}
	return AuraAbilitySystemComponent;
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (UAuraAbilitySystemComponent* LocalAbilitySystem = GetAuraAbilitySystemComponent())
	{
		LocalAbilitySystem->AbilityInputTagPressed(InputTag);
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (UAuraAbilitySystemComponent* LocalAbilitySystem = GetAuraAbilitySystemComponent())
	{
		LocalAbilitySystem->AbilityInputTagHeld(InputTag);
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (UAuraAbilitySystemComponent* LocalAbilitySystemComponent = GetAuraAbilitySystemComponent())
	{
		LocalAbilitySystemComponent->AbilityInputTagReleased(InputTag);
	}
}

void AAuraPlayerController::UpdateMagicCircleLocation() const
{
	if (IsValid(MagicCircle))
	{
		MagicCircle->SetActorLocation(CursorHit.ImpactPoint);
	}
}

bool AAuraPlayerController::IsTargetingEnemy() const
{
	return TargetingStatus == ETargetingStatus::TargetingEnemy;
}

bool AAuraPlayerController::IsTargetingOther() const
{
	return TargetingStatus == ETargetingStatus::TargetingOther;
}

bool AAuraPlayerController::IsNotTargeting() const
{
	return TargetingStatus == ETargetingStatus::NotTargeting;
}

void AAuraPlayerController::InitializeInputMode(const ECommonInputType NewInputMode)
{
	FInputModeGameAndUI InputModeData;
	switch (NewInputMode)
	{
	case ECommonInputType::MouseAndKeyboard:
		InputType = EAuraInputMode::MouseAndKeyboard;
		bShowMouseCursor = true;
		DefaultMouseCursor = EMouseCursor::Default;
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputModeData.SetHideCursorDuringCapture(false);
		SetInputMode(InputModeData);
		break;
	case ECommonInputType::Gamepad:
	default:
		InputType = EAuraInputMode::Gamepad;
		bShowMouseCursor = false;
		DefaultMouseCursor = EMouseCursor::Default;
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputModeData.SetHideCursorDuringCapture(true);
		SetInputMode(InputModeData);
		break;
	}
}
