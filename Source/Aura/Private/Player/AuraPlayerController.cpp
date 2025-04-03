// Copyright Alien Shores


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MovieSceneTracksComponentTypes.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Actor/MagicCircle.h"
#include "Aura/Aura.h"
#include "Character/EnemyInterface.h"
#include "Components/SplineComponent.h"
#include "Input/AuraInputComponent.h"
#include "Tags/AuraGameplayTags.h"
#include "UI/Widget/DamageTextComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	// Stop the game if AuraContext is not set
	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer()
	);
	if (Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
	AutoMove_Process();
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
		FAuraGameplayTags::Get().Player_Block_InputPressed
	))
	{
		return;
	}
	const FVector2D InputAxisVector = Value.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
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
		                                       : ECC_Visibility;
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
	if (GetAuraAbilitySystemComponent() && GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(
		FAuraGameplayTags::Get().Player_Block_InputPressed
	))
	{
		return;
	}
	if (FAuraGameplayTags::IsLeftMouseButton(InputTag))
	{
		bAutoRunning = false;
	}
	if (UAuraAbilitySystemComponent* LocalAbilitySystem = GetAuraAbilitySystemComponent())
	{
		LocalAbilitySystem->AbilityInputTagPressed(InputTag);
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (GetAuraAbilitySystemComponent() && GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(
		FAuraGameplayTags::Get().Player_Block_InputHeld
	))
	{
		return;
	}
	if (IsTargetingEnemy() || !FAuraGameplayTags::IsLeftMouseButton(InputTag))
	{
		if (UAuraAbilitySystemComponent* LocalAbilitySystem = GetAuraAbilitySystemComponent())
		{
			LocalAbilitySystem->AbilityInputTagHeld(InputTag);
		}
	}
	else if (IsNotTargeting())
	{
		FollowTime += GetWorld()->GetDeltaSeconds();
		if (CursorHit.bBlockingHit)
		{
			CachedDestination = CursorHit.ImpactPoint;
		}
		if (APawn* ControlledPawn = GetPawn<APawn>())
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetAuraAbilitySystemComponent() && GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(
		FAuraGameplayTags::Get().Player_Block_InputReleased
	))
	{
		return;
	}
	if (IsTargetingEnemy() || !FAuraGameplayTags::IsLeftMouseButton(InputTag))
	{
		if (UAuraAbilitySystemComponent* LocalAbilitySystemComponent = GetAuraAbilitySystemComponent())
		{
			LocalAbilitySystemComponent->AbilityInputTagReleased(InputTag);
		}
	}
	else if (IsNotTargeting())
	{
		AutoMove_Start();
	}
}

void AAuraPlayerController::AutoMove_Start()
{
	if (GetAuraAbilitySystemComponent() && GetAuraAbilitySystemComponent()->HasMatchingGameplayTag(
		FAuraGameplayTags::Get().Player_Block_InputPressed
	))
	{
		// do not automove if input is blocked
		return;
	}
	if (const APawn* ControlledPawn = GetPawn<APawn>(); FollowTime <= ShortPressThreshold)
	{
		// DEVNOTE: this only works in multiplayer if the Allow Client-side Navigation toggle is checked
		// in the Unreal Engine Project Settings. (see corresponding change in DefaultEngine.ini)
		if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
			this,
			ControlledPawn->GetActorLocation(),
			CachedDestination
		))
		{
			Spline->ClearSplinePoints();
			for (const FVector& PathPoint : NavPath->PathPoints)
			{
				Spline->AddSplinePoint(PathPoint, ESplineCoordinateSpace::World);
			}
			//
			if (NavPath->PathPoints.Num() > 0)
			{
				CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
				bAutoRunning = true;
			}
		}
		if (ClickNiagaraSystem)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ClickNiagaraSystem, CachedDestination);
		}
	}
	FollowTime = 0.f;
}

void AAuraPlayerController::AutoMove_Process()
{
	if (!bAutoRunning)
	{
		return;
	}
	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(
			ControlledPawn->GetActorLocation(),
			ESplineCoordinateSpace::World
		);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(
			LocationOnSpline,
			ESplineCoordinateSpace::World
		);
		ControlledPawn->AddMovementInput(Direction);
		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			AutoMove_End();
		}
	}
}

void AAuraPlayerController::AutoMove_End()
{
	bAutoRunning = false;
}

void AAuraPlayerController::UpdateMagicCircleLocation()
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
