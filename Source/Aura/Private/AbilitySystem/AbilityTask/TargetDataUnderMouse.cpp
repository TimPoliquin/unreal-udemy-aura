// Copyright Alien Shores


#include "AbilitySystem/AbilityTask/TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/AuraPlayerController.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(
	UGameplayAbility* OwningAbility
)
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
	if (const AAuraPlayerController* PlayerController = Cast<AAuraPlayerController>(Ability->GetCurrentActorInfo()->PlayerController.Get()))
	{
		FHitResult CursorHit;
		FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
		FGameplayAbilityTargetDataHandle DataHandle;
		if (PlayerController->IsInputTypeMouse())
		{
			PlayerController->GetHitResultUnderCursor(ECC_Target, false, CursorHit);
		}
		else
		{
			TArray<AActor*> ActorsToIgnore;
			UKismetSystemLibrary::SphereTraceSingle(
				GetAvatarActor(),
				GetAvatarActor()->GetActorLocation(),
				GetAvatarActor()->GetActorLocation() + GetAvatarActor()->GetActorForwardVector() * 10000,
				50,
				UEngineTypes::ConvertToTraceType(ECC_Target),
				false,
				ActorsToIgnore,
				EDrawDebugTrace::None,
				CursorHit,
				true
			);
		}
		Data->HitResult = CursorHit;
		DataHandle.Add(Data);
		// create a prediction window for this ability system
		FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());
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
			if (CursorHit.bBlockingHit)
			{
				HasMouseTarget.Broadcast(DataHandle);
			}
			else
			{
				HasNoTarget.Broadcast(DataHandle);
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
