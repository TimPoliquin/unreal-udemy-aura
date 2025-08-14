// Copyright Alien Shores


#include "Item/Pickup/AuraTreasurePickup.h"

#include "Game/AuraGameModeBase.h"
#include "Tags/AuraGameplayTags.h"


class UAuraTreasureConfig;

AAuraTreasurePickup::AAuraTreasurePickup()
{
	PrimaryActorTick.bCanEverTick = false;
	ItemType = FAuraGameplayTags::Get().Item_Type_Treasure;
}

AAuraTreasurePickup* AAuraTreasurePickup::SpawnTreasure(const AActor* WorldContextObject, const FVector& Location, const float Value)
{
	if (Value >= 1.f)
	{
		if (const UAuraTreasureConfig* TreasureConfig = AAuraGameModeBase::GetAuraGameMode(WorldContextObject)->GetTreasureConfig())
		{
			if (const FTreasureConfig TreasureConfigVal = TreasureConfig->GetTreasureConfig(Value, true); TreasureConfigVal.IsValid())
			{
				FTransform TreasureTransform;
				TreasureTransform.SetLocation(Location);
				TreasureTransform.SetRotation(WorldContextObject->GetActorRotation().Quaternion());
				AAuraTreasurePickup* TreasurePickup = WorldContextObject->GetWorld()->SpawnActorDeferred<AAuraTreasurePickup>(
					TreasureConfig->GetTreasureClass(),
					TreasureTransform,
					nullptr,
					nullptr,
					ESpawnActorCollisionHandlingMethod::AlwaysSpawn
				);
				TreasurePickup->Count = TreasureConfigVal.Value;
				TreasurePickup->MeshComponent->SetStaticMesh(TreasureConfigVal.Mesh);
				TreasurePickup->FinishSpawning(TreasureTransform);
				return TreasurePickup;
			}
		}
	}
	return nullptr;
}
