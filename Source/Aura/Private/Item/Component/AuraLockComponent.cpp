// Copyright Alien Shores


#include "Item/Component/AuraLockComponent.h"

#include "Aura/AuraLogChannels.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "LevelAssets/Switch/SwitchInterface.h"
#include "Player/InventoryActorInterface.h"


class UPlayerInventoryComponent;

EAuraLockSFXPlaybackLocation FAuraLockSFXConfig::GetPlaybackLocation(const EAuraLockSFXPlaybackLocation OverridePlaybackLocation) const
{
	switch (PlaybackLocation)
	{
	case EAuraLockSFXPlaybackLocation::Default:
		return PlaybackLocation;
	default:
		return OverridePlaybackLocation;
	}
}

void FAuraLockSFXConfig::AutoPlay(const AActor* WorldContextObject) const
{
	if (bAutoPlay)
	{
		Play(WorldContextObject);
	}
}

void FAuraLockSFXConfig::Play(const AActor* WorldContextObject, const EAuraLockSFXPlaybackLocation OverridePlaybackLocation) const
{
	FLoadSoftObjectPathAsyncDelegate OnLoadCompletedDelegate;
	OnLoadCompletedDelegate.BindLambda([this, OverridePlaybackLocation, WorldContextObject](FSoftObjectPath ObjectPath, UObject* LoadedSound)
	{
		switch (GetPlaybackLocation(OverridePlaybackLocation))
		{
		case EAuraLockSFXPlaybackLocation::Default:
		case EAuraLockSFXPlaybackLocation::UI:
			UGameplayStatics::PlaySound2D(WorldContextObject, SoundEffect.Get());
			break;
		case EAuraLockSFXPlaybackLocation::ActorLocation:
			UGameplayStatics::PlaySoundAtLocation(WorldContextObject, SoundEffect.Get(), WorldContextObject->GetActorLocation(), WorldContextObject->GetActorRotation());
			break;
		}
	});
	SoundEffect.LoadAsync(OnLoadCompletedDelegate);
}

UAuraLockComponent::UAuraLockComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	CompletedSound.PlaybackLocation = EAuraLockSFXPlaybackLocation::ActorLocation;
}

bool UAuraLockComponent::IsPreconditionMet(const AActor* Player) const
{
	switch (UnlockMode)
	{
	case EAuraUnlockMode::Unlocked:
		return true;
	case EAuraUnlockMode::Key:
		return IsPreconditionMet_Key(Player);
	case EAuraUnlockMode::Switch:
		return IsPreconditionMet_Switch();
	case EAuraUnlockMode::Custom:
		return IsPreconditionMet_Custom();
	default:
		return false;
	}
}

bool UAuraLockComponent::TryUnlock(AActor* Player)
{
	if (IsUnlocked())
	{
		return false;
	}
	switch (UnlockMode)
	{
	case EAuraUnlockMode::Unlocked:
		Unlock();
		break;
	case EAuraUnlockMode::Key:
		TryUnlock_Key(Player);
		break;
	case EAuraUnlockMode::Switch:
		TryUnlock_Switch();
		break;
	case EAuraUnlockMode::Custom:
		TryUnlock_Custom(Player);
		break;
	}
	return IsUnlocked();
}

bool UAuraLockComponent::IsUnlocked() const
{
	return UnlockMode == EAuraUnlockMode::Unlocked || bUnlocked;
}

bool UAuraLockComponent::IsLocked() const
{
	return UnlockMode != EAuraUnlockMode::Unlocked && !bUnlocked;
}

bool UAuraLockComponent::IsManuallyUnlockable() const
{
	return UnlockMode == EAuraUnlockMode::Key;
}

FString UAuraLockComponent::GetUnlockText() const
{
	return IsManuallyUnlockable() && IsLocked() ? UnlockText : OpenText;
}

bool UAuraLockComponent::IsUnlockedAlways() const
{
	return UnlockMode == EAuraUnlockMode::Unlocked;
}

bool UAuraLockComponent::IsUnlockedByKey() const
{
	return UnlockMode == EAuraUnlockMode::Key;
}

bool UAuraLockComponent::IsUnlockedBySwitch() const
{
	return UnlockMode == EAuraUnlockMode::Switch;
}

bool UAuraLockComponent::IsUnlockedByCustomLogic() const
{
	return UnlockMode == EAuraUnlockMode::Custom;
}

void UAuraLockComponent::SetUnlockMode(const EAuraUnlockMode InUnlockMode)
{
	UnlockMode = InUnlockMode;
}

void UAuraLockComponent::BeginPlay()
{
	Super::BeginPlay();
	switch (UnlockMode)
	{
	case EAuraUnlockMode::Unlocked:
		break;
	case EAuraUnlockMode::Key:
		InitializeUnlock_Key();
		break;
	case EAuraUnlockMode::Switch:
		InitializeUnlock_Switch();
		break;
	case EAuraUnlockMode::Custom:
		InitializeUnlock_Custom();
		break;
	}
}

void UAuraLockComponent::InitializeUnlock_Key_Implementation()
{
	if (!KeyTag.IsValid())
	{
		UE_LOG(LogAura, Warning, TEXT("[%s] Configured with an invalid key"), *GetName());
		return;
	}
	if (!AAuraGameModeBase::GetAuraGameMode(this)->FindItemDefinitionByItemTag(KeyTag).IsValid())
	{
		UE_LOG(LogAura, Warning, TEXT("[%s] Configured key is missing an item definition: %s"), *GetName(), *KeyTag.GetTagName().ToString());
	}
}

void UAuraLockComponent::InitializeUnlock_Switch_Implementation()
{
	if (Switches.Num() <= 0)
	{
		UE_LOG(LogAura, Warning, TEXT("[%s] No switches configured!"), *GetName());
		return;
	}
	for (AActor* SwitchActor : Switches)
	{
		if (!IsValid(SwitchActor))
		{
			continue;
		}
		if (ISwitchInterface* SwitchInterface = Cast<ISwitchInterface>(SwitchActor))
		{
			SwitchInterface->GetOnSwitchActivatedDelegate().AddUniqueDynamic(this, &UAuraLockComponent::OnSwitchActivated);
		}
		else
		{
			UE_LOG(LogAura, Warning, TEXT("[%s] Configured switch does not implement ISwitchInterface: %s"), *GetName(), *SwitchActor->GetName());
		}
	}
}

void UAuraLockComponent::InitializeUnlock_Custom_Implementation()
{
	// Do nothing by default here
}

bool UAuraLockComponent::IsPreconditionMet_Custom_Implementation() const
{
	return false;
}

bool UAuraLockComponent::IsPreconditionMet_Key_Implementation(const AActor* Player) const
{
	if (const UPlayerInventoryComponent* InventoryComponent = IInventoryActorInterface::GetInventoryComponent(Player))
	{
		return InventoryComponent->HasItemInInventory(KeyTag);
	}
	return false;
}

bool UAuraLockComponent::IsPreconditionMet_Switch_Implementation() const
{
	for (const AActor* SwitchActor : Switches)
	{
		if (!ISwitchInterface::IsSwitchActive(SwitchActor))
		{
			return false;
		}
	}
	return true;
}

void UAuraLockComponent::Unlock_Implementation(bool bBroadcast)
{
	bUnlocked = true;
	if (bBroadcast)
	{
		OnUnlockDelegate.Broadcast(FOnAuraLockComponentUnlockPayload(GetOwner(), this, UnlockMode));
	}
}

void UAuraLockComponent::TryUnlock_Key_Implementation(AActor* Player)
{
	bool bShouldUnlock = false;
	if (UPlayerInventoryComponent* InventoryComponent = IInventoryActorInterface::GetInventoryComponent(Player))
	{
		if (bConsumesKey)
		{
			bShouldUnlock = InventoryComponent->UseKey(KeyTag);
		}
		else
		{
			bShouldUnlock = InventoryComponent->HasItemInInventory(KeyTag);
		}
	}
	if (bShouldUnlock)
	{
		Unlock();
	}
}

void UAuraLockComponent::TryUnlock_Switch_Implementation()
{
	bool bAllActive = true;
	for (const AActor* SwitchActor : Switches)
	{
		if (!ISwitchInterface::IsSwitchActive(SwitchActor))
		{
			bAllActive = false;
			break;
		}
	}
	if (bAllActive)
	{
		Unlock();
	}
}

void UAuraLockComponent::TryUnlock_Custom_Implementation(AActor* Player)
{
	// by default, do nothing
}

void UAuraLockComponent::PlaySuccessSound(const EAuraLockSFXPlaybackLocation PlaybackLocation)
{
	SuccessSound.Play(GetOwner(), PlaybackLocation);
}

void UAuraLockComponent::PlayResetSound(const EAuraLockSFXPlaybackLocation PlaybackLocation)
{
	ResetSound.Play(GetOwner(), PlaybackLocation);
}

void UAuraLockComponent::PlayCompletedSound(const EAuraLockSFXPlaybackLocation PlaybackLocation)
{
	CompletedSound.Play(GetOwner(), PlaybackLocation);
}

void UAuraLockComponent::OnSwitchActivated(const FOnSwitchStatusChangedPayload& Payload)
{
	bool bAllActivated = true;
	bool PayloadSwitchFound = false;
	bool bTriggerReset = false;
	int32 UnlockedCount = 0;
	for (AActor* Switch : Switches)
	{
		if (Payload.Switch == Switch)
		{
			PayloadSwitchFound = true;
		}
		if (!ISwitchInterface::IsSwitchActive(Switch))
		{
			bAllActivated = false;
			if (bOrdered && !PayloadSwitchFound)
			{
				bTriggerReset = true;
				break;
			}
		}
		else
		{
			UnlockedCount++;
		}
	}
	if (bTriggerReset)
	{
		ResetSound.AutoPlay(Payload.Switch);
		OnResetDelegate.Broadcast();
	}
	else
	{
		SuccessSound.AutoPlay(Payload.Switch);
		OnUnlockProgressDelegate.Broadcast(FOnAuraLockComponentUnlockProgressPayload(GetOwner(), this, UnlockedCount, Switches.Num()));
	}
	if (bAllActivated)
	{
		CompletedSound.AutoPlay(GetOwner());
		TryUnlock_Switch();
	}
	else if (bTriggerReset)
	{
		for (AActor* Switch : Switches)
		{
			ISwitchInterface::ResetSwitch(Switch);
		}
	}
}
