// Copyright Alien Shores


#include "Fishing/AuraFishingBlueprintNode.h"

#include "Aura/AuraLogChannels.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Fishing/AuraFishInfo.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/FishingActorInterface.h"
#include "Interaction/FishingComponentInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Item/AuraItemBlueprintLibrary.h"
#include "Utils/RandUtils.h"


UAuraFishingBlueprintNode* UAuraFishingBlueprintNode::GoFishing(
	UObject* WorldContextObject,
	AActor* Player,
	const FAuraGoFishingParams& Params
)
{
	UAuraFishingBlueprintNode* BlueprintNode = NewObject<UAuraFishingBlueprintNode>();
	BlueprintNode->PlayerActor = Player;
	BlueprintNode->GoFishingParams = Params;
	// Register with the game instance to avoid being garbage collected
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UAuraFishingBlueprintNode::WaitForFishToBeLured()
{
	FTimerDelegate InterestTimerDelegate;
	InterestTimerDelegate.BindLambda(
		[this]()
		{
			OnFishingLuredDelegate.Broadcast(PlayerActor);
		}
	);
	PlayerActor->GetWorld()->GetTimerManager().SetTimer(
		FishInterestToLureTimerHandle,
		InterestTimerDelegate,
		GoFishingParams.TimeToLure.Value(),
		false
	);
}

void UAuraFishingBlueprintNode::LureAndWaitForABite(const FGameplayTag& FishType)
{
	ActiveFishType = FishType;
	SetFishState(EFishState::Lured);
	FTimerDelegate LureCallbackDelegate;
	LureCallbackDelegate.BindLambda(
		[this]()
		{
			BiteAndWaitForPlayerOrFlee();
		}
	);
	PlayerActor->GetWorld()->GetTimerManager().SetTimer(
		FishLureToBiteTimerHandle,
		LureCallbackDelegate,
		GoFishingParams.LureToBiteTime.Value(),
		false
	);
}

void UAuraFishingBlueprintNode::BiteAndWaitForPlayerOrFlee()
{
	SetFishState(EFishState::Biting);
	OnFishingBiteDelegate.Broadcast(PlayerActor);
	FTimerDelegate BiteToFleeCallbackDelegate;
	BiteToFleeCallbackDelegate.BindLambda(
		[this]()
		{
			Flee();
		}
	);
	PlayerActor->GetWorld()->GetTimerManager().SetTimer(
		FishBiteToFleeTimerHandle,
		BiteToFleeCallbackDelegate,
		GoFishingParams.BiteToFleeTime.Value(),
		false
	);
}

void UAuraFishingBlueprintNode::Flee()
{
	ActiveFishType = FGameplayTag::EmptyTag;
	SetFishState(EFishState::Fled);
	OnFishingFishHasFledDelegate.Broadcast(PlayerActor);
}

void UAuraFishingBlueprintNode::Reel()
{
	PlayerActor->GetWorld()->GetTimerManager().ClearTimer(FishBiteToFleeTimerHandle);
	SetFishState(EFishState::Fighting);
	OnFishingFishReelingDelegate.Broadcast(PlayerActor);
}

void UAuraFishingBlueprintNode::Catch()
{
	CaughtFish = UAuraItemBlueprintLibrary::ToFishCatch(PlayerActor, ActiveFishType);
	SetFishState(EFishState::Caught);
	OnFishingFishCaughtDelegate.Broadcast(PlayerActor);
}


void UAuraFishingBlueprintNode::Activate()
{
	if (const TScriptInterface<IFishingComponentInterface> FishingComponent =
		IFishingActorInterface::GetFishingComponent(
			PlayerActor
		))
	{
		FishingComponent->SetupForFishing(GoFishingParams.FishingTarget->GetActorLocation());
		FishingComponent->GetOnFishingStateChangedDelegate().AddDynamic(
			this,
			&UAuraFishingBlueprintNode::OnFishingStateChanged
		);
	}
	if (!IFishingComponentInterface::IsFishing(PlayerActor))
	{
		MoveCameraToPosition();
		MovePlayerToPosition();
	}
	else
	{
		OnPlayerInPositionDelegate.Broadcast(PlayerActor);
	}
}

void UAuraFishingBlueprintNode::Cleanup()
{
	ActiveFishType = FGameplayTag::EmptyTag;
	OnCameraInPositionDelegate.Clear();
	OnFishingCancelledDelegate.Clear();
	OnPlayerInPositionDelegate.Clear();
	OnFishingRodEquippedDelegate.Clear();
	OnFishingRodCastDelegate.Clear();
	OnFishingLuredDelegate.Clear();
	OnFishingBiteDelegate.Clear();
	OnFishingFishHasFledDelegate.Clear();
	OnFishingFishReelingDelegate.Clear();
	OnFishingFishCaughtDelegate.Clear();
	PlayerActor->GetWorld()->GetTimerManager().ClearTimer(PlayerMoveToTargetTimerHandle);
	PlayerActor->GetWorld()->GetTimerManager().ClearTimer(FishInterestToLureTimerHandle);
	PlayerActor->GetWorld()->GetTimerManager().ClearTimer(FishLureToBiteTimerHandle);
	PlayerActor->GetWorld()->GetTimerManager().ClearTimer(FishBiteToFleeTimerHandle);
}

void UAuraFishingBlueprintNode::PrepareForContinue()
{
	SetFishState(EFishState::None);
	ActiveFishType = FGameplayTag::EmptyTag;
	Cleanup();
	if (const TScriptInterface<IFishingComponentInterface> FishingComponent =
		IFishingActorInterface::GetFishingComponent(
			PlayerActor
		))
	{
		FishingComponent->GetOnFishingStateChangedDelegate().RemoveDynamic(
			this,
			&UAuraFishingBlueprintNode::OnFishingStateChanged
		);
		FishingComponent->PrepareForContinue();
	}
}

void UAuraFishingBlueprintNode::End()
{
	OnFishingCancelledDelegate.Broadcast(PlayerActor);
	Cleanup();
	if (const TScriptInterface<IFishingComponentInterface> FishingComponent =
		IFishingActorInterface::GetFishingComponent(
			PlayerActor
		))
	{
		FishingComponent->GetOnFishingStateChangedDelegate().RemoveDynamic(
			this,
			&UAuraFishingBlueprintNode::OnFishingStateChanged
		);
		FishingComponent->EndFishing();
	}
	SetReadyToDestroy();
}

void UAuraFishingBlueprintNode::MoveCameraToPosition()
{
	if (IPlayerInterface* PlayerInterface = Cast<IPlayerInterface>(PlayerActor))
	{
		OnCameraMoveFinishedDelegate.BindUObject(this, &UAuraFishingBlueprintNode::OnCameraInPosition);
		PlayerInterface->MoveCameraToPointWithCallback(
			GoFishingParams.CameraTarget->GetActorLocation(),
			GoFishingParams.CameraTarget->GetActorForwardVector(),
			GoFishingParams.CameraMovementCurve,
			OnCameraMoveFinishedDelegate
		);
	}
}

void UAuraFishingBlueprintNode::MovePlayerToPosition()
{
	if (APawn* Pawn = Cast<APawn>(PlayerActor))
	{
		// TODO - do something more robust here via the IFishingInterface
		PlayerMoveToTargetTimerDelegate.BindLambda(
			[this]()
			{
				OnPlayerInPositionDelegate.Broadcast(PlayerActor);
			}
		);
		UAIBlueprintHelperLibrary::SimpleMoveToActor(Pawn->GetController(), GoFishingParams.PlayerTarget);
		Pawn->GetWorld()->GetTimerManager().SetTimer(
			PlayerMoveToTargetTimerHandle,
			PlayerMoveToTargetTimerDelegate,
			1.f,
			false
		);
		ICombatInterface::UpdateFacingTarget(Pawn, GoFishingParams.FishingTarget->GetActorLocation());
	}
}


void UAuraFishingBlueprintNode::OnCameraInPosition()
{
	OnCameraInPositionDelegate.Broadcast(PlayerActor);
}

void UAuraFishingBlueprintNode::OnFishingRodEquipped()
{
	OnFishingRodEquippedDelegate.Broadcast(PlayerActor);
}

void UAuraFishingBlueprintNode::OnFishingRodCast()
{
	OnFishingRodCastDelegate.Broadcast(PlayerActor);
	WaitForFishToBeLured();
}

void UAuraFishingBlueprintNode::OnFishingStateChanged(EFishingState FishingState)
{
	UE_LOG(LogAura, Warning, TEXT("Fishing state has changed: [%d]"), FishingState);
	switch (FishingState)
	{
	case EFishingState::Equipped:
		OnFishingRodEquipped();
		break;
	case EFishingState::Cast:
		OnFishingRodCast();
		break;
	case EFishingState::Reeling:
		Reel();
		break;
	default:
		break;
	}
}

void UAuraFishingBlueprintNode::SetFishState(const EFishState& InFishState)
{
	ActiveFishState = InFishState;
	IFishingComponentInterface::FishStateChanged(PlayerActor, InFishState);
}
