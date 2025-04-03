// Copyright Alien Shores


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "Aura/AuraLogChannels.h"
#include "Game/AuraGameInstance.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadScreen::InitializeLoadSlots()
{
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_0->SetLoadSlotName(TEXT("LoadSlot_0"));
	LoadSlot_0->SetSlotIndex(0);
	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_1->SetLoadSlotName(TEXT("LoadSlot_1"));
	LoadSlot_1->SetSlotIndex(1);
	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_2->SetLoadSlotName(TEXT("LoadSlot_2"));
	LoadSlot_2->SetSlotIndex(2);
	LoadSlots.Add(LoadSlot_0);
	LoadSlots.Add(LoadSlot_1);
	LoadSlots.Add(LoadSlot_2);
}

void UMVVM_LoadScreen::NewSlotButtonPressed(const int32 SlotIndex, const FString& EnteredName)
{
	AAuraGameModeBase* AuraGameModeBase = AAuraGameModeBase::GetAuraGameMode(this);
	if (!IsValid(AuraGameModeBase))
	{
		GEngine->AddOnScreenDebugMessage(1, 15, FColor::Red, TEXT("Please switch to single player"));
		return;
	}
	UMVVM_LoadSlot* LoadSlot = GetLoadSlotByIndex(SlotIndex);
	if (EnteredName.IsEmpty())
	{
		LoadSlot->SetPlayerName(FString::Printf(TEXT("Player %d"), SlotIndex + 1));
	}
	else
	{
		LoadSlot->SetPlayerName(EnteredName);
	}
	LoadSlot->SetLoadSlotStatus(Taken);
	LoadSlot->SetMapAssetName(AuraGameModeBase->GetDefaultMapAssetName());
	LoadSlot->SetMapName(AuraGameModeBase->GetDefaultMapName());
	LoadSlot->SetPlayerStartTag(AuraGameModeBase->GetDefaultPlayerStartTag());
	LoadSlot->SetPlayerLevel(AuraGameModeBase->GetDefaultPlayerLevel());
	AuraGameModeBase->SaveSlotData(LoadSlot);
	LoadSlot->InitializeSlot();

	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(AuraGameModeBase->GetGameInstance());
	AuraGameInstance->LoadSlotName = LoadSlot->GetLoadSlotName();
	AuraGameInstance->LoadSlotIndex = LoadSlot->GetSlotIndex();
	AuraGameInstance->PlayerStartTag = AuraGameModeBase->GetDefaultPlayerStartTag();
	SelectSlotButtonPressed(SlotIndex);
}

void UMVVM_LoadScreen::NewGameButtonPressed(const int32 SlotIndex)
{
	GetLoadSlotByIndex(SlotIndex)->ShowEnterName();
}

void UMVVM_LoadScreen::SelectSlotButtonPressed(const int32 SlotIndex)
{
	for (const UMVVM_LoadSlot* LoadSlot : LoadSlots)
	{
		LoadSlot->EnableSelectSlotButtonDelegate.Broadcast(LoadSlot->GetSlotIndex() != SlotIndex);
	}
	if (SlotIndex >= 0)
	{
		SelectedSlot = GetLoadSlotByIndex(SlotIndex);
		SlotSelectedDelegate.Broadcast();
	}
}

void UMVVM_LoadScreen::DeleteButtonPressed()
{
	if (IsValid(SelectedSlot))
	{
		AAuraGameModeBase::DeleteSlot(SelectedSlot->GetLoadSlotName(), SelectedSlot->GetSlotIndex());
		SelectedSlot->SetLoadSlotStatus(Vacant);
		SelectedSlot->InitializeSlot();
		ClearSelectedSlot();
	}
}

void UMVVM_LoadScreen::ClearSelectedSlot()
{
	SelectedSlot = nullptr;
	for (const UMVVM_LoadSlot* LoadSlot : LoadSlots)
	{
		LoadSlot->EnableSelectSlotButtonDelegate.Broadcast(true);
	}
}

void UMVVM_LoadScreen::PlayButtonPressed()
{
	if (IsValid(SelectedSlot))
	{
		AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
		UAuraGameInstance* AuraGameInstance = AuraGameModeBase->GetAuraGameInstance();
		AuraGameInstance->PlayerStartTag = SelectedSlot->GetPlayerStartTag();
		AuraGameInstance->LoadSlotIndex = SelectedSlot->GetSlotIndex();
		AuraGameInstance->LoadSlotName = SelectedSlot->GetLoadSlotName();
		AuraGameModeBase->LoadMap(SelectedSlot);
	}
}

void UMVVM_LoadScreen::LoadData()
{
	const AAuraGameModeBase* AuraGameModeBase = AAuraGameModeBase::GetAuraGameMode(this);
	if (!IsValid(AuraGameModeBase))
	{
		return;
	}
	for (UMVVM_LoadSlot* LoadSlot : LoadSlots)
	{
		const UAuraSaveGame* SaveGame = AuraGameModeBase->GetSaveSlotData(
			LoadSlot->GetLoadSlotName(),
			LoadSlot->GetSlotIndex()
		);
		LoadSlot->SetPlayerName(SaveGame->PlayerName);
		LoadSlot->SetPlayerLevel(SaveGame->PlayerLevel);
		LoadSlot->SetMapName(SaveGame->MapName);
		LoadSlot->SetLoadSlotStatus(SaveGame->SaveSlotStatus);
		LoadSlot->SetPlayerStartTag(SaveGame->PlayerStartTag);
		LoadSlot->InitializeSlot();
	}
}
