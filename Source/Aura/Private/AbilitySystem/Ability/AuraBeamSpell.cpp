// Copyright Alien Shores


#include "AbilitySystem/Ability/AuraBeamSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AbilityTask/TargetDataUnderMouse.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Aura/AuraLogChannels.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Interaction/CombatInterface.h"
#include "Tags/AuraGameplayTags.h"

void UAuraBeamSpell::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!GetAvatarActorFromActorInfo()->Implements<UCombatInterface>())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	if (UTargetDataUnderMouse* TargetDataUnderMouseTask = UTargetDataUnderMouse::CreateTargetDataUnderMouse(this))
	{
		TargetDataUnderMouseTask->HasMouseTarget.AddDynamic(this, &UAuraBeamSpell::OnReceiveMouseData);
		ExecuteTask(TargetDataUnderMouseTask);
	}
}

void UAuraBeamSpell::SetMouseCursorVisible(const bool Visible) const
{
	if (CurrentActorInfo && CurrentActorInfo->PlayerController.IsValid())
	{
		CurrentActorInfo->PlayerController->SetShowMouseCursor(Visible);
	}
}

void UAuraBeamSpell::SetMovementEnabled(const bool Enabled) const
{
	const ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	UCharacterMovementComponent* CharacterMovementComponent = Character
		                                                          ? Cast<UCharacterMovementComponent>(
			                                                          Character->GetMovementComponent()
		                                                          )
		                                                          : nullptr;
	if (CharacterMovementComponent)
	{
		if (!Enabled)
		{
			CharacterMovementComponent->DisableMovement();
		}
		else
		{
			CharacterMovementComponent->SetMovementMode(MOVE_Walking);
		}
	}
}

void UAuraBeamSpell::OnReceiveMouseData(const FGameplayAbilityTargetDataHandle& DataHandle)
{
	if (const FHitResult HitResult = *DataHandle.Data[0].Get()->GetHitResult(); HitResult.bBlockingHit)
	{
		ExecuteAbility(HitResult);
	}
	else
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UAuraBeamSpell::OnMontageEventReceived(FGameplayEventData Payload)
{
	SetMovementEnabled(false);
	ICombatInterface::SetActiveAbilityTag(GetAvatarActorFromActorInfo(), GetDefaultAbilityTag());
}

void UAuraBeamSpell::ExecuteAbility(const FHitResult& HitResult)
{
	if (UAbilityTask_WaitInputRelease* WaitInputRelease = UAbilityTask_WaitInputRelease::WaitInputRelease(this))
	{
		WaitInputRelease->OnRelease.AddDynamic(this, &UAuraBeamSpell::OnInputRelease);
		ExecuteTask(WaitInputRelease);
	}
	SetMouseCursorVisible(false);
	ICombatInterface::UpdateFacingTarget(GetAvatarActorFromActorInfo(), HitResult.ImpactPoint);
	if (UAbilityTask_PlayMontageAndWait* PlayMontageAndWait =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			AbilityMontage
		))
	{
		ExecuteTask(PlayMontageAndWait);
	}
	if (UAbilityTask_WaitGameplayEvent* WaitGameplayEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		EventTag,
		nullptr,
		true,
		true
	))
	{
		WaitGameplayEvent->EventReceived.AddDynamic(this, &UAuraBeamSpell::OnMontageEventReceived);
		ExecuteTask(WaitGameplayEvent);
	}
}

void UAuraBeamSpell::OnInputRelease(float TimeHeld)
{
	SetMouseCursorVisible(true);
	SetMovementEnabled(true);
	EndAbility(GetCurrentAbilitySpec()->Handle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
