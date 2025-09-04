// Copyright Alien Shores


#include "Game/AuraGameState.h"

#include "Kismet/GameplayStatics.h"
#include "Player/AuraInventoryComponent.h"
#include "Player/Progression/AuraProgressionComponent.h"

AAuraGameState::AAuraGameState()
{
	SetNetUpdateFrequency(100.f);
	InventoryComponent = CreateDefaultSubobject<UAuraInventoryComponent>(TEXT("PlayerInventoryComponent"));
	ProgressionComponent = CreateDefaultSubobject<UAuraProgressionComponent>(TEXT("ProgressionComponent"));
}

void AAuraGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

AAuraGameState* AAuraGameState::Get(const UObject* WorldContextObject)
{
	if (IsValid(WorldContextObject))
	{
		return Cast<AAuraGameState>(UGameplayStatics::GetGameState(WorldContextObject));
	}
	return nullptr;
}

UAuraInventoryComponent* AAuraGameState::GetInventoryComponent() const
{
	return InventoryComponent;
}

UAuraProgressionComponent* AAuraGameState::GetProgressionComponent() const
{
	return ProgressionComponent;
}
