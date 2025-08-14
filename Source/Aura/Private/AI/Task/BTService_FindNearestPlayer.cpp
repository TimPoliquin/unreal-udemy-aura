// Copyright Alien Shores


#include "AI/Task/BTService_FindNearestPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/EnemyInterface.h"
#include "Game/AuraGameInstance.h"
#include "Game/AuraGameModeBase.h"

UBTService_FindNearestPlayer::UBTService_FindNearestPlayer()
{
	NodeName = "Find Nearest Player";
}

void UBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	float ClosestDistance = FLT_MAX;
	AActor* NearestPlayer = nullptr;
	if (const AAIController* AIController = OwnerComp.GetAIOwner())
	{
		if (const APawn* ControlledPawn = AIController->GetPawn())
		{
			const TArray<AActor*> PlayerActors = AAuraGameModeBase::GetAuraGameMode(ControlledPawn)->GetAuraGameInstance()->GetActivePlayerActors();
			for (AActor* PlayerActor : PlayerActors)
			{
				const float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), PlayerActor->GetActorLocation());
				if (Distance < ClosestDistance)
				{
					ClosestDistance = Distance;
					NearestPlayer = PlayerActor;
				}
			}
		}
	}
	if (OwnerComp.GetBlackboardComponent())
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetToFollowSelector.SelectedKeyName, NearestPlayer);
		OwnerComp.GetBlackboardComponent()->SetValueAsFloat(DistanceToTargetSelector.SelectedKeyName, ClosestDistance);
	}
}
