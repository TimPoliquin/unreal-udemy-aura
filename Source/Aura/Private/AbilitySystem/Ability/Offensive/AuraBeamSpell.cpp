// Copyright Alien Shores


#include "AbilitySystem/Ability/AuraBeamSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AbilityTask/TargetDataUnderMouse.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Interaction/CombatInterface.h"
#include "GameplayCueFunctionLibrary.h"
#include "ToolMenusEditor.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/AuraLogChannels.h"
#include "Components/TimelineComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utils/TagUtils.h"

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
	if (UAbilityTask_WaitInputRelease* WaitInputRelease = UAbilityTask_WaitInputRelease::WaitInputRelease(this))
	{
		WaitInputRelease->OnRelease.AddDynamic(this, &UAuraBeamSpell::OnInputRelease);
		ExecuteTask(WaitInputRelease);
	}
	if (UTargetDataUnderMouse* TargetDataUnderMouseTask = UTargetDataUnderMouse::CreateTargetDataUnderMouse(
		this
	))
	{
		TargetDataUnderMouseTask->HasMouseTarget.AddDynamic(this, &UAuraBeamSpell::OnReceiveMouseData);
		ExecuteTask(TargetDataUnderMouseTask);
	}
}

void UAuraBeamSpell::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	SetMouseCursorVisible(true);
	SetMovementEnabled(true);
	EndAbilityOnTargets();
	if (GetWorld()->GetTimerManager().IsTimerActive(TimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	}
	if (GetWorld()->GetTimerManager().IsTimerActive(DelayTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(DelayTimerHandle);
	}
	CommitAbilityCooldown(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false);
	PrimaryTarget = nullptr;
	CueActors.Empty();
	ActorGameplayCueParameters.Empty();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
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

AActor* UAuraBeamSpell::DetermineCueTarget(AActor* ActorHit) const
{
	if (IsValid(ActorHit) && ActorHit->Implements<UCombatInterface>() && !TagUtils::HasAnyTag(
		ActorHit,
		ICombatInterface::GetTargetTagsToIgnore(GetAvatarActorFromActorInfo())
	))
	{
		return ActorHit;
	}
	return GetAvatarActorFromActorInfo();
}

void UAuraBeamSpell::DetermineCascadingTargets(AActor* CueTarget, TArray<AActor*>& OutCascadedTargets)
{
	const int32 CascadingTargetCount = GetCascadeTargetsCount();
	if (CascadingTargetCount <= 0)
	{
		return;
	}
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
	if (IsValid(CueTarget))
	{
		ActorsToIgnore.AddUnique(CueTarget);
	}
	TArray<AActor*> ActorsInRange;
	UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
		GetAvatarActorFromActorInfo(),
		ActorsToIgnore,
		ICombatInterface::GetTargetTagsToIgnore(GetAvatarActorFromActorInfo()),
		HitLocation,
		GetCascadeRadius(),
		ActorsInRange
	);
	UAuraAbilitySystemLibrary::GetClosestActors(
		CascadingTargetCount,
		HitLocation,
		ActorsInRange,
		OutCascadedTargets
	);
}

int32 UAuraBeamSpell::GetCascadeTargetsCount() const
{
	if (bForceCascadeMax)
	{
		return MaxCascadeTargets;
	}
	return FMath::Min(GetAbilityLevel() - 1, MaxCascadeTargets);
}

void UAuraBeamSpell::ApplyCueToTarget(AActor* CueTarget, const FGameplayCueParameters& Parameters)
{
	UGameplayCueFunctionLibrary::AddGameplayCueOnActor(
		CueTarget,
		LoopCueTag,
		Parameters
	);
	ActorGameplayCueParameters.Add(CueTarget, Parameters);
	CueActors.AddUnique(CueTarget);
}

void UAuraBeamSpell::CascadeToActor(const AActor* FromActor, AActor* CascadeTarget)
{
	FGameplayCueParameters TargetCueParams = FGameplayCueParameters();
	TargetCueParams.TargetAttachComponent = FromActor->GetRootComponent();
	TargetCueParams.Location = CascadeTarget->GetActorLocation();
	TargetCueParams.SourceObject = CascadeTarget;
	ApplyCueToTarget(CascadeTarget, TargetCueParams);
}

void UAuraBeamSpell::InitializeTimer()
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UAuraBeamSpell::OnTimerTick, DamageTick, true);
}

void UAuraBeamSpell::ApplyDamage(AActor* DamageActor)
{
	if (!IsValid(DamageActor) || ICombatInterface::IsDead(DamageActor) || !HasAuthority(
		&CurrentActivationInfo
	))
	{
		return;
	}
	FMakeEffectSpecSignature ApplyDamageDelegate;
	ApplyDamageDelegate.BindLambda(
		[this](FGameplayEffectSpecHandle& Spec)
		{
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
				Spec,
				DamageConfig.DamageTypeTag,
				GetDamageAtLevel(GetAbilityLevel())
			);
		}
	);
	UAuraAbilitySystemLibrary::ApplyGameplayEffectSpec(
		CurrentActorInfo->AbilitySystemComponent.Get(),
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(DamageActor),
		DamageEffectClass,
		GetAbilityLevel(),
		&ApplyDamageDelegate
	);
}

void UAuraBeamSpell::OnTimerTick()
{
	if (CommitAbilityCost(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
	{
		if (!HasAuthority(&CurrentActivationInfo))
		{
			return;
		}
		TArray ApplyDamageActors(CueActors);
		ApplyDamageActors.RemoveAll(
			[this](AActor* Actor)
			{
				return Actor == GetAvatarActorFromActorInfo() || ICombatInterface::IsDead(Actor);
			}
		);

		for (AActor* Actor : ApplyDamageActors)
		{
			ApplyDamage(Actor);
		}
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
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

void UAuraBeamSpell::SpawnBeam(FGameplayEventData Payload)
{
	SetMovementEnabled(false);
	ICombatInterface::SetActiveAbilityTag(GetAvatarActorFromActorInfo(), GetDefaultAbilityTag());
	AActor* ActorHit = TraceFirstTarget(HitLocation);
	bool bIsEnemyHit = (IsValid(ActorHit) && ActorHit->Implements<UCombatInterface>());
	FGameplayCueParameters FirstTargetCueParams = FGameplayCueParameters();
	FirstTargetCueParams.TargetAttachComponent = ICombatInterface::GetWeapon(GetAvatarActorFromActorInfo());
	FirstTargetCueParams.Location = bIsEnemyHit
		                                ? ActorHit->GetActorLocation()
		                                : HitLocation;
	FirstTargetCueParams.SourceObject = ActorHit;
	if (bIsEnemyHit)
	{
		PrimaryTarget = ActorHit;
		BindPrimaryTargetDeath(ActorHit);
	}
	// if the hit actor is valid, implements the combat interface, and is an enemy target...
	if (AActor* CueTarget = DetermineCueTarget(ActorHit); IsValid(CueTarget))
	{
		ApplyCueToTarget(CueTarget, FirstTargetCueParams);
		if (bIsEnemyHit)
		{
			TArray<AActor*> CascadingTargets;
			DetermineCascadingTargets(CueTarget, CascadingTargets);
			for (AActor* CascadingTarget : CascadingTargets)
			{
				BindCascadeTargetDeath(CascadingTarget);
				CascadeToActor(CueTarget, CascadingTarget);
			}
		}
		InitializeTimer();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

void UAuraBeamSpell::ExecuteAbility(const FHitResult& HitResult)
{
	HitLocation = HitResult.ImpactPoint;
	SetMouseCursorVisible(false);
	ICombatInterface::UpdateFacingTarget(GetAvatarActorFromActorInfo(), HitResult.ImpactPoint);
	FGameplayCueParameters CueParams = FGameplayCueParameters();
	UGameplayCueFunctionLibrary::ExecuteGameplayCueOnActor(GetAvatarActorFromActorInfo(), SoundCueTag, CueParams);
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
		WaitGameplayEvent->EventReceived.AddDynamic(this, &UAuraBeamSpell::SpawnBeam);
		ExecuteTask(WaitGameplayEvent);
	}
}

AActor* UAuraBeamSpell::TraceFirstTarget(const FVector& BeamTargetLocation)
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
	FHitResult HitResult;
	const FVector SocketLocation = ICombatInterface::GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
	UKismetSystemLibrary::SphereTraceSingle(
		GetAvatarActorFromActorInfo(),
		SocketLocation,
		BeamTargetLocation,
		BeamTraceSize,
		TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		HitResult,
		true
	);
	if (HitResult.bBlockingHit)
	{
		HitLocation = HitResult.ImpactPoint;
		return HitResult.GetActor();
	}
	return nullptr;
}

void UAuraBeamSpell::OnDelayedRelease()
{
	EndAbility(GetCurrentAbilitySpec()->Handle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UAuraBeamSpell::OnInputRelease(float TimeHeld)
{
	if (TimeHeld < MinimumSpellTime)
	{
		// Enforce minimum spell time
		GetWorld()->GetTimerManager().SetTimer(
			DelayTimerHandle,
			this,
			&UAuraBeamSpell::OnDelayedRelease,
			MinimumSpellTime - TimeHeld,
			false
		);
	}
	else
	{
		EndAbility(GetCurrentAbilitySpec()->Handle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UAuraBeamSpell::OnPrimaryTargetDead(AActor* TargetActor)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UAuraBeamSpell::OnCascadeTargetDead(AActor* CascadeTarget)
{
	if (ActorGameplayCueParameters.Contains(CascadeTarget))
	{
		UGameplayCueFunctionLibrary::RemoveGameplayCueOnActor(
			CascadeTarget,
			LoopCueTag,
			ActorGameplayCueParameters[CascadeTarget]
		);
	}
	ActorGameplayCueParameters.Remove(CascadeTarget);
	CueActors.Remove(CascadeTarget);
}

void UAuraBeamSpell::BindPrimaryTargetDeath(AActor* Actor)
{
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Actor))
	{
		if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::OnPrimaryTargetDead))
		{
			CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::OnPrimaryTargetDead);
		}
	}
}

void UAuraBeamSpell::BindCascadeTargetDeath(AActor* Actor)
{
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Actor))
	{
		if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::OnCascadeTargetDead))
		{
			CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::OnCascadeTargetDead);
		}
	}
}

bool UAuraBeamSpell::IsTargetALivingEnemy(const AActor* TargetActor) const
{
	return ICombatInterface::IsAlive(TargetActor) && !TagUtils::HasAnyTag(
		TargetActor,
		ICombatInterface::GetTargetTagsToIgnore(GetAvatarActorFromActorInfo())
	);
}

void UAuraBeamSpell::EndAbilityOnTargets()
{
	if (CueActors.Num() <= 0)
	{
		return;
	}
	for (TArray<AActor*> Targets(CueActors); AActor* CueActor : Targets)
	{
		UGameplayCueFunctionLibrary::RemoveGameplayCueOnActor(
			CueActor,
			LoopCueTag,
			ActorGameplayCueParameters[CueActor]
		);
		if (IsTargetALivingEnemy(CueActor) && HasAuthority(&CurrentActivationInfo))
		{
			FDamageEffectParams LastHit = MakeDamageEffectParamsFromClassDefaults(CueActor);
			UAuraAbilitySystemLibrary::ApplyDamageEffect(LastHit);
		}
	}
}
