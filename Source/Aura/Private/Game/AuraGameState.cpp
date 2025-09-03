// Copyright Alien Shores


#include "Game/AuraGameState.h"

#include "Kismet/GameplayStatics.h"
#include "Player/PlayerInventoryComponent.h"

AAuraGameState::AAuraGameState()
{
	PlayerInventoryComponent = CreateDefaultSubobject<UPlayerInventoryComponent>(TEXT("PlayerInventoryComponent"));
}

void AAuraGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// nothing directly needed here.
}

AAuraGameState* AAuraGameState::Get(const UObject* WorldContextObject)
{
	if (IsValid(WorldContextObject))
	{
		return Cast<AAuraGameState>(UGameplayStatics::GetGameState(WorldContextObject));
	}
	return nullptr;
}

UPlayerInventoryComponent* AAuraGameState::GetPlayerInventoryComponent() const
{
	return PlayerInventoryComponent;
}

void AAuraGameState::FromSaveData(const UAuraSaveGame* SaveData)
{
	PlayerInventoryComponent->FromSaveData(SaveData);
}

void AAuraGameState::ToSaveData(UAuraSaveGame* SaveData) const
{
	PlayerInventoryComponent->ToSaveData(SaveData);
}
