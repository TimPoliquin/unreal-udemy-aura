// Copyright Alien Shores


#include "Player/AuraPlayerState.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Game/AuraSaveGame.h"
#include "Net/UnrealNetwork.h"
#include "Player/PlayerInventoryComponent.h"

AAuraPlayerState::AAuraPlayerState()
{
	SetNetUpdateFrequency(100.f);
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>(TEXT("AttributeSet"));
	PlayerInventoryComponent = CreateDefaultSubobject<UPlayerInventoryComponent>(TEXT("PlayerInventoryComponent"));
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

UPlayerInventoryComponent* AAuraPlayerState::GetInventoryComponent_Implementation() const
{
	return PlayerInventoryComponent;
}

int32 AAuraPlayerState::GetCharacterLevel() const
{
	return Level;
}

void AAuraPlayerState::InitializeLevel(const int32 InLevel)
{
	Level = InLevel;
	OnLevelInitializedDelegate.Broadcast(Level);
}

void AAuraPlayerState::SetLevel(const int32 NewLevel)
{
	Level = NewLevel;
	OnLevelChangeDelegate.Broadcast(Level);
}

void AAuraPlayerState::AddToLevel(const int32 AddLevel)
{
	SetLevel(Level + AddLevel);
}

int32 AAuraPlayerState::GetXP() const
{
	return XP;
}

void AAuraPlayerState::AddToXP(const int32 InXP)
{
	SetXP(InXP + XP);
}

int32 AAuraPlayerState::GetAttributePoints() const
{
	return AttributePoints;
}

void AAuraPlayerState::AddAttributePoints(const int32 InAttributePoints)
{
	SetAttributePoints(AttributePoints + InAttributePoints);
}

int32 AAuraPlayerState::GetSpellPoints() const
{
	return SpellPoints;
}

void AAuraPlayerState::AddSpellPoints(const int32 InSpellPoints)
{
	SetSpellPoints(SpellPoints + InSpellPoints);
}

void AAuraPlayerState::SetSpellPoints(const int32 InSpellPoints)
{
	SpellPoints = InSpellPoints;
	OnSpellPointsChangeDelegate.Broadcast(SpellPoints);
}

void AAuraPlayerState::SetAttributePoints(const int32 InAttributePoints)
{
	AttributePoints = InAttributePoints;
	OnAttributePointsChangeDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::SetXP(const int32 InXP)
{
	XP = InXP;
	OnXPChangeDelegate.Broadcast(XP);
}

void AAuraPlayerState::FromSaveData(const UAuraSaveGame* SaveData)
{
	InitializeLevel(SaveData->PlayerLevel);
	SetXP(SaveData->PlayerXP);
	SetAttributePoints(SaveData->AttributePoints);
	SetSpellPoints(SaveData->SpellPoints);
	AttributeSet->FromSaveData(SaveData);
	AbilitySystemComponent->FromSaveData(SaveData);
	PlayerInventoryComponent->FromSaveData(SaveData);
}

void AAuraPlayerState::ToSaveData(UAuraSaveGame* SaveData) const
{
	SaveData->PlayerLevel = GetCharacterLevel();
	SaveData->PlayerXP = GetXP();
	SaveData->AttributePoints = GetAttributePoints();
	SaveData->SpellPoints = GetSpellPoints();
	AttributeSet->ToSaveData(SaveData);
	AbilitySystemComponent->ToSaveData(SaveData);
	PlayerInventoryComponent->ToSaveData(SaveData);
}

void AAuraPlayerState::InitializeAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());
}

void AAuraPlayerState::OnRep_Level(int32 OldLevel) const
{
	OnLevelChangeDelegate.Broadcast(Level);
}

void AAuraPlayerState::OnRep_XP(int32 OldXP) const
{
	OnXPChangeDelegate.Broadcast(XP);
}

void AAuraPlayerState::OnRep_AttributePoints(int32 InAttributePoints) const
{
	OnAttributePointsChangeDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::OnRep_SpellPoints(int32 InSpellPoints) const
{
	OnSpellPointsChangeDelegate.Broadcast(SpellPoints);
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
