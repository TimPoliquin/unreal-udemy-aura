// Copyright Alien Shores


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "Game/Subsystem/LevelGameInstanceSubsystem.h"
#include "Game/Subsystem/SaveGameSubsystem.h"
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
	USaveGameSubsystem* SaveGameSubsystem = USaveGameSubsystem::Get(LocalPlayer);
	UMVVM_LoadSlot* LoadSlot = GetLoadSlotByIndex(SlotIndex);
	FString PlayerName = EnteredName.IsEmpty()
		                     ? FString::Printf(TEXT("Player %d"), SlotIndex + 1)
		                     : EnteredName;
	UAuraSaveGame* SaveGame = SaveGameSubsystem->CreateDefaultSaveData(SlotIndex, LoadSlot->GetLoadSlotName());
	SaveGame->PlayerName = PlayerName;
	SaveGameSubsystem->SaveSlotData(SaveGame);
	SaveGameSubsystem->InitializeSaveState(SaveGame, false);
	LoadSlot->FromSaveGame(SaveGame);
	LoadSlot->InitializeSlot();
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
		USaveGameSubsystem::Get(GetLocalPlayer())->DeleteSlot(
			SelectedSlot->GetLoadSlotName(),
			SelectedSlot->GetSlotIndex()
		);
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
		USaveGameSubsystem* SaveGameSubsystem = USaveGameSubsystem::Get(GetLocalPlayer());
		SaveGameSubsystem->InitializeSaveState(
			SelectedSlot->GetPlayerStartTag(),
			SelectedSlot->GetLoadSlotName(),
			SelectedSlot->GetSlotIndex(),
			false
		);
		ULevelGameInstanceSubsystem::Get(GetLocalPlayer())->LoadMap(GetLocalPlayer(), SelectedSlot->GetMapName());
	}
}

void UMVVM_LoadScreen::LoadData()
{
	if (const USaveGameSubsystem* SaveGameSubsystem = USaveGameSubsystem::Get(GetLocalPlayer()))
	{
		for (UMVVM_LoadSlot* LoadSlot : LoadSlots)
		{
			LoadSlot->FromSaveGame(
				SaveGameSubsystem->GetSaveSlotData(
					LoadSlot->GetLoadSlotName(),
					LoadSlot->GetSlotIndex()
				)
			);
		}
	}
}

ULocalPlayer* UMVVM_LoadScreen::GetLocalPlayer() const
{
	return LocalPlayer;
}

void UMVVM_LoadScreen::SetLocalPlayer(class ULocalPlayer* InLocalPlayer)
{
	LocalPlayer = InLocalPlayer;
}
