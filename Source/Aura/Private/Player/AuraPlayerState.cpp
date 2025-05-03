// Copyright Alien Shores


#include "Player/AuraPlayerState.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Game/AuraSaveGame.h"
#include "Net/UnrealNetwork.h"

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
	DOREPLIFETIME(AAuraPlayerState, Level);
	DOREPLIFETIME(AAuraPlayerState, XP);
	DOREPLIFETIME(AAuraPlayerState, AttributePoints);
	DOREPLIFETIME(AAuraPlayerState, SpellPoints);
}

UAttributeSet* AAuraPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}

void AAuraPlayerState::FromSaveData(const UAuraSaveGame* SaveData)
{
	InitializeLevel(SaveData->PlayerLevel);
	SetXP(SaveData->PlayerXP);
	SetAttributePoints(SaveData->AttributePoints);
	SetSpellPoints(SaveData->SpellPoints);
}

void AAuraPlayerState::ToSaveData(UAuraSaveGame* SaveData) const
{
	SaveData->PlayerLevel = GetCharacterLevel();
	SaveData->PlayerXP = GetXP();
	SaveData->AttributePoints = GetAttributePoints();
	SaveData->SpellPoints = GetSpellPoints();
}

float AAuraPlayerState::GetXPToNextLevelPercentage() const
{
	checkf(LevelUpInfo, TEXT("LevelUpInfo not set on AuraPlayerState - this must be set in the Blueprint"))
	return LevelUpInfo->GetLevelProgressPercentage(XP);
}

int32 AAuraPlayerState::FindLevelByXP(const int32 InXP) const
{
	checkf(LevelUpInfo, TEXT("LevelUpInfo not set on AuraPlayerState - this must be set in the Blueprint"))
	return LevelUpInfo->FindLevelByXP(InXP);
}

FAuraLevelUpRewards AAuraPlayerState::GetLevelUpRewards(int32 CurrentLevel) const
{
	checkf(LevelUpInfo, TEXT("LevelUpInfo not set on AuraPlayerState - this must be set in the Blueprint"))
	return LevelUpInfo->GetRewardsByLevel(CurrentLevel);
}
