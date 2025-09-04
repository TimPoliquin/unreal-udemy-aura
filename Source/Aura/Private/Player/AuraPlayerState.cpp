// Copyright Alien Shores


#include "Player/AuraPlayerState.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

AAuraPlayerState::AAuraPlayerState()
{
	SetNetUpdateFrequency(100.f);
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

UAttributeSet* AAuraPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}


// void AAuraPlayerState::FromSaveData(const UOLD_AuraSaveGame* SaveData)
// {
// 	AttributeSet->FromSaveData(SaveData);
// 	AbilitySystemComponent->FromSaveData(SaveData);
// }
//
// void AAuraPlayerState::ToSaveData(UOLD_AuraSaveGame* SaveData) const
// {
// 	AttributeSet->ToSaveData(SaveData);
// 	AbilitySystemComponent->ToSaveData(SaveData);
// }

void AAuraPlayerState::InitializeAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());
}
