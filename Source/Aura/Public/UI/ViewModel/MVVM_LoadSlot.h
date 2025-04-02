﻿// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Game/AuraSaveGame.h"
#include "MVVM_LoadSlot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWidgetSwitcherIndexSignature, const int32, WidgetSwitcherIndex);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnableSelectSlotButtonSignature, const bool, bEnable);

/**
 * 
 */
UCLASS()
class AURA_API UMVVM_LoadSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FSetWidgetSwitcherIndexSignature SetWidgetSwitcherIndexDelegate;
	UPROPERTY(BlueprintAssignable)
	FEnableSelectSlotButtonSignature EnableSelectSlotButtonDelegate;

	void ShowEnterName();
	void InitializeSlot();

	FString GetLoadSlotName() const
	{
		return LoadSlotName;
	}

	int32 GetSlotIndex() const
	{
		return SlotIndex;
	}

	void SetSlotIndex(const int32 InSlotIndex)
	{
		this->SlotIndex = InSlotIndex;
	}

	FString GetPlayerName() const
	{
		return PlayerName;
	}

	void SetPlayerName(const FString& InPlayerName);
	void SetLoadSlotName(const FString& InLoadSlotName);
	FString GetMapName() const;
	void SetMapName(const FString& InMapName);
	TEnumAsByte<ESaveSlotStatus> GetLoadSlotStatus() const;
	void SetLoadSlotStatus(const TEnumAsByte<ESaveSlotStatus>& InLoadSlotStatus);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess="true"))
	FString LoadSlotName;
	UPROPERTY()
	int32 SlotIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess="true"))
	FString PlayerName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess="true"))
	int32 PlayerLevel;

public:
	int32 GetPlayerLevel() const;
	void SetPlayerLevel(const int32 InPlayerLevel);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess="true"))
	FString MapName;
	UPROPERTY()
	FName PlayerStartTag;

public:
	FName GetPlayerStartTag() const;
	void SetPlayerStartTag(const FName& InPlayerStartTag);

private:
	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> LoadSlotStatus;
};
