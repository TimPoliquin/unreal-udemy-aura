// Copyright Alien Shores


#include "Item/Chest/AuraTreasureChestActor.h"

#include "IDetailTreeNode.h"
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

bool AAuraTreasureChestActor::HandleInteract_Implementation(AActor* Player)
{
	Unlock(Player);
	return true;
}

bool AAuraTreasureChestActor::IsUnlocked() const
{
	return LockComponent->IsUnlocked();
}

bool AAuraTreasureChestActor::Unlock(AActor* Player)
{
	if (LockComponent->IsPreconditionMet(Player))
	{
		if (LockComponent->TryUnlock(Player))
		{
			PlayUnlockEffect(false);
			DisablePOI();
			return true;
		}
	}
	return false;
}

void AAuraTreasureChestActor::LoadActor_Implementation()
{
	if (HasActorBegunPlay() && LockComponent->IsUnlocked())
	{
		PlayUnlockEffect(true);
		DisablePOI();
	}
}

void AAuraTreasureChestActor::BeginPlay()
{
	Super::BeginPlay();
	if (LockComponent->IsUnlocked())
	{
		PlayUnlockEffect(true);
		DisablePOI();
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
