// Copyright Alien Shores


#include "AbilitySystem/AbilityTask/TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"
#include "Aura/Aura.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* Task = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return Task;
}

void UTargetDataUnderMouse::Activate()
{
	if (Ability->GetCurrentActorInfo()->IsLocallyControlled())
	{
		SendMouseCursorDataToServer();
	}
	else
	{
		FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		FPredictionKey PredictionKey = GetActivationPredictionKey();
		// Add callback for when server receives target mouse data 
		AbilitySystemComponent
			.Get()
			->AbilityTargetDataSetDelegate(SpecHandle, PredictionKey)
			.AddUObject(this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallback);
		// invoke callback if it's already received the target mouse data
		if (!AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, PredictionKey))
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UTargetDataUnderMouse::SendMouseCursorDataToServer() const
{
	if (const APlayerController* PlayerController = Ability->GetCurrentActorInfo()->PlayerController.Get())
	{
		if (FHitResult CursorHit; PlayerController->GetHitResultUnderCursor(ECC_Target, false, CursorHit))
		{
			// create a prediction window for this ability system
			FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());
			FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
			FGameplayAbilityTargetDataHandle DataHandle;
			Data->HitResult = CursorHit;
			DataHandle.Add(Data);
			// replicate data to server
			AbilitySystemComponent->ServerSetReplicatedTargetData(
				GetAbilitySpecHandle(),
				GetActivationPredictionKey(),
				DataHandle,
				FGameplayTag(),
				AbilitySystemComponent->ScopedPredictionKey
			);
			// broadcast the ability locally if enabled
			if (ShouldBroadcastAbilityTaskDelegates())
			{
				HasMouseTarget.Broadcast(DataHandle);
			}
		}
	}
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(
	const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag GameplayTag
) const
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		HasMouseTarget.Broadcast(DataHandle);
	}
}
