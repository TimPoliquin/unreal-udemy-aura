// Copyright Alien Shores


#include "Fishing/AuraFishingBlueprintNode.h"

#include "Aura/AuraLogChannels.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/FishingActorInterface.h"
#include "Interaction/FishingComponentInterface.h"
#include "Interaction/PlayerInterface.h"


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
	MoveCameraToPosition();
	MovePlayerToPosition();
}

void UAuraFishingBlueprintNode::End()
{
	OnFishingCancelledDelegate.Broadcast(PlayerActor);
	PlayerActor->GetWorld()->GetTimerManager().ClearTimer(PlayerMoveToTargetTimerHandle);
	if (TScriptInterface<IFishingComponentInterface> FishingComponent = IFishingActorInterface::GetFishingComponent(
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

void UAuraFishingBlueprintNode::OnFishingRodCast()
{
	OnFishingRodCastDelegate.Broadcast(PlayerActor);
}

void UAuraFishingBlueprintNode::OnFishingRodEquipped()
{
	OnFishingRodEquippedDelegate.Broadcast(PlayerActor);
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
	default:
		break;
	}
}
