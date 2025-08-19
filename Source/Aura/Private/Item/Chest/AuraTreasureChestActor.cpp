// Copyright Alien Shores


#include "Item/Chest/AuraTreasureChestActor.h"

#include "Actor/Spawn/AuraSpawnBlueprintLibrary.h"
#include "Item/Component/AuraLockComponent.h"
#include "Item/Effect/SpawnEffectInterface.h"
#include "Item/Pickup/AuraPickupItemBase.h"
#include "Item/Pickup/AuraTreasurePickup.h"
#include "Item/Pickup/TieredItemInterface.h"
#include "Utils/ArrayUtils.h"


AAuraTreasureChestActor::AAuraTreasureChestActor()
{
	PrimaryActorTick.bCanEverTick = false;
	LockComponent = CreateDefaultSubobject<UAuraLockComponent>(TEXT("LockComponent"));
}

bool AAuraTreasureChestActor::IsUnlocked() const
{
	return LockComponent->IsUnlocked();
}

void AAuraTreasureChestActor::HandleInitialState()
{
	if (State == EAuraTreasureChestState::Locked && LockComponent->IsUnlocked())
	{
		State = EAuraTreasureChestState::Unlocked;
	}
	switch (State)
	{
	case EAuraTreasureChestState::Locked:
		// do nothing
		break;
	case EAuraTreasureChestState::Unlocked:
		// do nothing
		PlayUnlockEffect(true);
		break;
	case EAuraTreasureChestState::Open:
		PlayOpenEffect(true);
		DisablePOI();
		break;
	}
}

void AAuraTreasureChestActor::LoadActor_Implementation()
{
	if (HasActorBegunPlay())
	{
		HandleInitialState();
	}
}

void AAuraTreasureChestActor::BeginPlay()
{
	Super::BeginPlay();
	HandleInitialState();
	if (!LockComponent->IsUnlocked())
	{
		LockComponent->OnUnlockDelegate.AddUniqueDynamic(this, &AAuraTreasureChestActor::OnChestUnlocked);
	}
}

bool AAuraTreasureChestActor::IsPreconditionMet_Implementation(AActor* Player) const
{
	switch (State)
	{
	case EAuraTreasureChestState::Locked:
		return LockComponent->IsPreconditionMet(Player);
	default:
		return true;
	}
}

void AAuraTreasureChestActor::HandleInteract_Implementation(AActor* Player)
{
	switch (State)
	{
	case EAuraTreasureChestState::Locked:
		if (LockComponent->IsManuallyUnlockable() && LockComponent->TryUnlock(Player))
		{
			PlayOpenEffect(false);
		}
		break;
	case EAuraTreasureChestState::Unlocked:
		PlayOpenEffect(false);
		break;
	case EAuraTreasureChestState::Open:
		// do nothing
		break;
	}
}

FTransform AAuraTreasureChestActor::GetRewardInitialSpawnLocation_Implementation() const
{
	return GetActorTransform();
}

TArray<FAuraLootInstance> AAuraTreasureChestActor::InstantiateRewardActors()
{
	TArray<FTransform> Transforms = UAuraSpawnBlueprintLibrary::GenerateSpawnLocations(
		GetActorLocation(),
		SpawnDistance,
		GoldAmount > 0 ? LootDefinitions.Num() + 1 : LootDefinitions.Num(),
		120.f,
		GetActorForwardVector(),
		GetActorUpVector()
	);
	TArray<FAuraLootInstance> RewardActors;
	UArrayUtils::ShuffleArray(Transforms);
	const FTransform InitialTransform = GetRewardInitialSpawnLocation();
	for (int32 LootDefinitionIndex = 0; LootDefinitionIndex < LootDefinitions.Num(); LootDefinitionIndex++)
	{
		const FAuraLootDefinition& LootDefinition = LootDefinitions[LootDefinitionIndex];
		const FTransform& TargetTransform = Transforms[LootDefinitionIndex];
		if (AAuraPickupItemBase* Item = GetWorld()->SpawnActorDeferred<AAuraPickupItemBase>(LootDefinition.ItemClass, InitialTransform, nullptr, nullptr,
		                                                                                    ESpawnActorCollisionHandlingMethod::AlwaysSpawn))
		{
			ITieredItemInterface::SetItemLevel(Item, LootDefinition.Level);
			if (RewardAnimatorClass)
			{
				AActor* Animator = ISpawnEffectInterface::CreateSpawnEffectActor(this, RewardAnimatorClass, Item, true);
				Item->FinishSpawning(InitialTransform);
				RewardActors.Add(FAuraLootInstance(Animator, InitialTransform, TargetTransform));
			}
			else
			{
				RewardActors.Add(FAuraLootInstance(Item, InitialTransform, TargetTransform));
			}
		}
	}
	if (GoldAmount > 0)
	{
		const FTransform TargetTransform = RewardAnimatorClass ? Transforms[Transforms.Num() - 1] : InitialTransform;
		if (AAuraTreasurePickup* GoldActor = AAuraTreasurePickup::SpawnTreasure(this, InitialTransform.GetLocation(), GoldAmount, false))
		{
			if (RewardAnimatorClass)
			{
				AActor* Animator = ISpawnEffectInterface::CreateSpawnEffectActor(this, RewardAnimatorClass, GoldActor, true);
				RewardActors.Add(FAuraLootInstance(Animator, InitialTransform, TargetTransform));
			}
			else
			{
				RewardActors.Add(FAuraLootInstance(GoldActor, TargetTransform));
			}
		}
	}
	return RewardActors;
}

FAuraLootInstance AAuraTreasureChestActor::Pop(TArray<FAuraLootInstance>& LootInstances)
{
	return LootInstances.Pop();
}

void AAuraTreasureChestActor::OnChestUnlocked(const FOnAuraLockComponentUnlockPayload& Payload)
{
	if (State == EAuraTreasureChestState::Locked)
	{
		State = EAuraTreasureChestState::Unlocked;
		if (Payload.UnlockType != EAuraUnlockMode::Key)
		{
			// Only play the unlock effect if the chest was unlocked by means other than a key.
			// If a key was used to unlock the chest, we'll want to jump straight to opening the chest.
			PlayUnlockEffect(false);
		}
	}
}

void AAuraTreasureChestActor::PlaySpawnReward_Implementation(const FAuraLootInstance& RewardInstance)
{
	if (RewardInstance.LootItem.IsValid())
	{
		TArray<AActor*> ItemChildActors;
		AActor* LootItemActor = RewardInstance.LootItem.Get();
		LootItemActor->SetActorHiddenInGame(false);
		LootItemActor->GetAttachedActors(ItemChildActors);
		for (AActor* ChildActor : ItemChildActors)
		{
			ChildActor->SetActorHiddenInGame(false);
		}
		if (LootItemActor->Implements<USpawnEffectInterface>())
		{
			ISpawnEffectInterface::PlaySpawnEffect(LootItemActor, RewardInstance.SpawnTransform, RewardInstance.TargetTransform, HeightMultiplier);
		}
		else
		{
			LootItemActor->SetActorLocation(RewardInstance.TargetTransform.GetLocation());
			LootItemActor->SetActorRotation(RewardInstance.TargetTransform.GetRotation());
		}
	}
}
