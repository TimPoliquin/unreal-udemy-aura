// Copyright Alien Shores


#include "LevelAssets/Switch/AuraSwitchTracker.h"

#include "Actor/AuraLockedInterface.h"
#include "Item/Component/AuraLockComponent.h"


// Sets default values
AAuraSwitchTracker::AAuraSwitchTracker()
{
	PrimaryActorTick.bCanEverTick = false;
	LockComponent = CreateDefaultSubobject<UAuraLockComponent>(TEXT("LockComponent"));
	LockComponent->SetUnlockMode(EAuraUnlockMode::Switch);
}

// Called when the game starts or when spawned
void AAuraSwitchTracker::BeginPlay()
{
	Super::BeginPlay();
	LockComponent->OnUnlockDelegate.AddDynamic(this, &AAuraSwitchTracker::OnUnlock);
	LockComponent->OnUnlockProgressDelegate.AddDynamic(this, &AAuraSwitchTracker::OnUnlockProgress);
	LockComponent->OnResetDelegate.AddDynamic(this, &AAuraSwitchTracker::OnReset);
}

void AAuraSwitchTracker::PlaySuccessSound(const EAuraLockSFXPlaybackLocation PlaybackLocation)
{
	LockComponent->PlaySuccessSound(PlaybackLocation);
}

void AAuraSwitchTracker::PlayResetSound(const EAuraLockSFXPlaybackLocation PlaybackLocation)
{
	LockComponent->PlayResetSound(PlaybackLocation);
}

void AAuraSwitchTracker::PlayCompletedSound(const EAuraLockSFXPlaybackLocation PlaybackLocation)
{
	LockComponent->PlayCompletedSound(PlaybackLocation);
}


void AAuraSwitchTracker::OnUnlock_Implementation(const FOnAuraLockComponentUnlockPayload& Payload)
{
	for (AActor* Actor : LockedActors)
	{
		IAuraLockedInterface::Unlock(Actor);
	}
}

void AAuraSwitchTracker::OnUnlockProgress_Implementation(const FOnAuraLockComponentUnlockProgressPayload& Payload)
{
	PlaySuccessSound();
}

void AAuraSwitchTracker::OnReset_Implementation()
{
	PlayResetSound();
}
