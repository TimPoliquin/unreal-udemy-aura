// Copyright Alien Shores


#include "Player/AuraPlayerState.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Player/Progression/AuraProgressionComponent.h"

AAuraPlayerState::AAuraPlayerState()
{
	SetNetUpdateFrequency(100.f);
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AbilitySystemComponent->bShouldSave = true;
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

UAuraAbilitySystemComponent* AAuraPlayerState::GetAuraAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAuraAttributeSet* AAuraPlayerState::GetAuraAttributeSet() const
{
	return AttributeSet;
}

int32 AAuraPlayerState::GetCharacterLevel_Implementation() const
{
	if (const UAuraProgressionComponent* ProgressionComponent = UAuraProgressionComponent::Get(this))
	{
		return ProgressionComponent->GetCharacterLevel();
	}
	return -1;
}

TArray<uint8> AAuraPlayerState::SaveData_Implementation()
{
	TArray<uint8> SaveData;
	FMemoryWriter Writer(SaveData);
	FAuraAttributeSetSaveData AttributeSetSaveData;
	AttributeSet->ToSaveData(AttributeSetSaveData);
	Writer << AttributeSetSaveData;
	return SaveData;
}

bool AAuraPlayerState::LoadData_Implementation(const TArray<uint8>& InData)
{
	try
	{
		if (InData.Num() <= 0)
		{
			return false;
		}
		FMemoryReader Reader(InData);
		FAuraAttributeSetSaveData AttributeSetSaveData;
		Reader << AttributeSetSaveData;
		AttributeSet->FromSaveData(AttributeSetSaveData);
		return true;
	}
	catch (...)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] Failed to deserialize attribute data"), *GetName());
		return false;
	}
}

void AAuraPlayerState::InitializeAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());
}

void AAuraPlayerState::BeginPlay()
{
	Super::BeginPlay();
	if (const UAuraProgressionComponent* ProgressionComponent = UAuraProgressionComponent::Get(this))
	{
		AttributeSet->InitializeDefaultAttributes(ProgressionComponent->GetCharacterLevel());
	}
}
