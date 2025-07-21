// Copyright Alien Shores


#include "Actor/Spawn/AuraActorTrackerComponent.h"
#include "Actor/Spawn/TrackableInterface.h"


UAuraActorTrackerComponent::UAuraActorTrackerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAuraActorTrackerComponent::Track(AActor* Actor)
{
	if (IsValid(Actor))
	{
		const int32 OldCount = Actors.Num();
		Actors.AddUnique(Actor);
		if (Actor->Implements<UTrackableInterface>())
		{
			Cast<ITrackableInterface>(Actor)->GetStopTrackingDelegate().AddUniqueDynamic(this, &UAuraActorTrackerComponent::OnTrackedActorDestroyed);
		}
		else
		{
			Actor->OnDestroyed.AddUniqueDynamic(this, &UAuraActorTrackerComponent::OnTrackedActorDestroyed);
		}
		OnCountChanged.Broadcast(FOnAuraActorTrackerCountChangedPayload(OldCount, Actors.Num()));
	}
}

void UAuraActorTrackerComponent::OnTrackedActorDestroyed(AActor* DestroyedActor)
{
	const int32 OldCount = Actors.Num();
	Actors.RemoveAll([DestroyedActor](const AActor* TrackedActor)
	{
		return TrackedActor == DestroyedActor;
	});
	OnCountChanged.Broadcast(FOnAuraActorTrackerCountChangedPayload(OldCount, Actors.Num()));
}
