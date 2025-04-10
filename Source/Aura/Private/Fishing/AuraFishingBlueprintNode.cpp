// Copyright Alien Shores


#include "Fishing/AuraFishingBlueprintNode.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/FishingInterface.h"
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
	if (IFishingInterface* FishingInterface = Cast<IFishingInterface>(PlayerActor))
	{
		FishingInterface->GetOnFishingRodCastDelegate().AddDynamic(this, &UAuraFishingBlueprintNode::OnFishingRodCast);
		FishingInterface->GetOnFishingRodEquippedDelegate().AddDynamic(
			this,
			&UAuraFishingBlueprintNode::OnFishingRodEquipped
		);
	}
	MoveCameraToPosition();
	MovePlayerToPosition();
}

void UAuraFishingBlueprintNode::End()
{
	OnFishingCancelledDelegate.Broadcast(PlayerActor);
	PlayerActor->GetWorld()->GetTimerManager().ClearTimer(PlayerMoveToTargetTimerHandle);
	if (IFishingInterface* FishingInterface = Cast<IFishingInterface>(PlayerActor))
	{
		FishingInterface->GetOnFishingRodCastDelegate().RemoveDynamic(
			this,
			&UAuraFishingBlueprintNode::OnFishingRodCast
		);
		FishingInterface->GetOnFishingRodEquippedDelegate().RemoveDynamic(
			this,
			&UAuraFishingBlueprintNode::OnFishingRodEquipped
		);
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
