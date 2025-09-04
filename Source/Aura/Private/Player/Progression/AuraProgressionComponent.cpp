// Copyright Alien Shores


#include "Player/Progression/AuraProgressionComponent.h"

#include "Game/AuraGameState.h"
#include "Game/Save/AuraSaveGameBlueprintFunctionLibrary.h"
#include "Game/Subsystem/AuraGameDataSubsystem.h"
#include "Net/UnrealNetwork.h"


UAuraProgressionComponent* UAuraProgressionComponent::Get(const UObject* WorldContextObject)
{
	if (const AAuraGameState* GameState = AAuraGameState::Get(WorldContextObject))
	{
		return GameState->GetProgressionComponent();
	}
	return nullptr;
}

UAuraProgressionComponent::UAuraProgressionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	AttributePoints = 0;
	SpellPoints = 0;
	SaveID = UAuraSaveGameBlueprintFunctionLibrary::GenerateSaveID(this);
	SetIsReplicatedByDefault(true);
}

void UAuraProgressionComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UAuraProgressionComponent, Level);
	DOREPLIFETIME(UAuraProgressionComponent, XP);
	DOREPLIFETIME(UAuraProgressionComponent, AttributePoints);
	DOREPLIFETIME(UAuraProgressionComponent, SpellPoints);
}

int32 UAuraProgressionComponent::GetCharacterLevel() const
{
	return Level;
}

TArray<uint8> UAuraProgressionComponent::SaveData_Implementation()
{
	return SerializeActorComponent();
}

bool UAuraProgressionComponent::LoadData_Implementation(const TArray<uint8>& Data)
{
	return DeserializeActorComponent(Data);
}

FString UAuraProgressionComponent::GetSaveID_Implementation() const
{
	return SaveID;
}

void UAuraProgressionComponent::InitializeLevel(const int32 InLevel)
{
	Level = InLevel;
	OnLevelInitializedDelegate.Broadcast(Level);
}

void UAuraProgressionComponent::SetLevel(const int32 NewLevel)
{
	Level = NewLevel;
	OnLevelChangeDelegate.Broadcast(Level);
}

void UAuraProgressionComponent::AddToLevel(const int32 AddLevel)
{
	SetLevel(Level + AddLevel);
}

int32 UAuraProgressionComponent::GetXP() const
{
	return XP;
}

void UAuraProgressionComponent::AddToXP(const int32 InXP)
{
	SetXP(InXP + XP);
}

int32 UAuraProgressionComponent::GetAttributePoints() const
{
	return AttributePoints;
}

void UAuraProgressionComponent::AddAttributePoints(const int32 InAttributePoints)
{
	SetAttributePoints(AttributePoints + InAttributePoints);
}

int32 UAuraProgressionComponent::GetSpellPoints() const
{
	return SpellPoints;
}

void UAuraProgressionComponent::AddSpellPoints(const int32 InSpellPoints)
{
	SetSpellPoints(SpellPoints + InSpellPoints);
}

int32 UAuraProgressionComponent::FindLevelByXP(const int32 InXP) const
{
	if (const UAuraGameDataSubsystem* GameData = UAuraGameDataSubsystem::Get(this))
	{
		return GameData->FindLevelByXP(InXP);
	}
	return -1;
}

FAuraLevelUpRewards UAuraProgressionComponent::GetLevelUpRewards(const int32 InLevel) const
{
	if (const UAuraGameDataSubsystem* GameData = UAuraGameDataSubsystem::Get(this))
	{
		return GameData->GetLevelUpRewards(InLevel);
	}
	return FAuraLevelUpRewards();
}

void UAuraProgressionComponent::SetSpellPoints(const int32 InSpellPoints)
{
	SpellPoints = InSpellPoints;
	OnSpellPointsChangeDelegate.Broadcast(SpellPoints);
}

void UAuraProgressionComponent::SetAttributePoints(const int32 InAttributePoints)
{
	AttributePoints = InAttributePoints;
	OnAttributePointsChangeDelegate.Broadcast(AttributePoints);
}

void UAuraProgressionComponent::SetXP(const int32 InXP)
{
	XP = InXP;
	OnXPChangeDelegate.Broadcast(XP);
}

void UAuraProgressionComponent::OnRep_Level(int32 OldLevel) const
{
	OnLevelChangeDelegate.Broadcast(Level);
}

void UAuraProgressionComponent::OnRep_XP(int32 OldXP) const
{
	OnXPChangeDelegate.Broadcast(XP);
}

void UAuraProgressionComponent::OnRep_AttributePoints(int32 InAttributePoints) const
{
	OnAttributePointsChangeDelegate.Broadcast(AttributePoints);
}

void UAuraProgressionComponent::OnRep_SpellPoints(int32 InSpellPoints) const
{
	OnSpellPointsChangeDelegate.Broadcast(SpellPoints);
}

TArray<uint8> UAuraProgressionComponent::SerializeActorComponent()
{
	TArray<uint8> Data;
	FMemoryWriter Writer(Data);

	// Create save data struct
	FAuraProgressionComponentSaveData SaveData;
	SaveData.Level = Level;
	SaveData.XP = XP;
	SaveData.AttributePoints = AttributePoints;
	SaveData.SpellPoints = SpellPoints;

	// Serialize the struct
	Writer << SaveData.Level;
	Writer << SaveData.XP;
	Writer << SaveData.AttributePoints;
	Writer << SaveData.SpellPoints;

	return Data;
}

bool UAuraProgressionComponent::DeserializeActorComponent(const TArray<uint8>& Data)
{
	if (Data.Num() == 0)
	{
		return false;
	}

	FMemoryReader Reader(Data);

	try
	{
		Reader << Level;
		Reader << XP;
		Reader << AttributePoints;
		Reader << SpellPoints;

		return true;
	}
	catch (...)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s:%s] Failed to deserialize equipment data"), *GetOwner()->GetName(), *GetName());
		return false;
	}
}
