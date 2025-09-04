// Copyright Alien Shores


#include "Fishing/AuraFishingPOI.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "FishingSuccessWidgetInterface.h"
#include "Fishing/AuraWaterRipple.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Aura/AuraLogChannels.h"
#include "Components/AudioComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/TargetPoint.h"
#include "Fishing/AuraFishCatch.h"
#include "Fishing/AuraFishInfo.h"
#include "Fishing/AuraFishingBlueprintNode.h"
#include "Fishing/AuraFishingComponent.h"
#include "Game/Subsystem/AuraGameDataSubsystem.h"
#include "Interaction/FishingActorInterface.h"
#include "Tags/AuraGameplayTags.h"
#include "UI/Library/AuraWidgetFunctionLibrary.h"
#include "UI/Widget/AuraFishingMessageWidget.h"
#include "UI/Widget/AuraInteractionWidget.h"
#include "UI/Widget/InteractionWidgetInterface.h"


class UAuraInventoryComponent;
// Sets default values
AAuraFishingPOI::AAuraFishingPOI()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	FishingTarget = CreateDefaultSubobject<UChildActorComponent>(TEXT("FishingTarget"));
	FishingTarget->SetChildActorClass(ATargetPoint::StaticClass());
	FishingTarget->SetupAttachment(GetRootComponent());
	Ripple = CreateDefaultSubobject<UChildActorComponent>(TEXT("Ripple"));
	Ripple->SetChildActorClass(AAuraWaterRipple::StaticClass());
	Ripple->SetupAttachment(FishingTarget);
	SplashFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SplashFX Component"));
	SplashFXComponent->SetupAttachment(FishingTarget);
	SplashSFXComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("SplashSFX Component"));
	SplashSFXComponent->SetupAttachment(SplashFXComponent);
	PlayerTarget = CreateDefaultSubobject<UChildActorComponent>(TEXT("PlayerTarget"));
	PlayerTarget->SetChildActorClass(ATargetPoint::StaticClass());
	PlayerTarget->SetupAttachment(GetRootComponent());
	CameraTarget = CreateDefaultSubobject<UChildActorComponent>(TEXT("CameraTarget"));
	CameraTarget->SetChildActorClass(ATargetPoint::StaticClass());
	CameraTarget->SetupAttachment(GetRootComponent());
}

TArray<FWeightedFish> AAuraFishingPOI::GetCurrentlyAvailableFish(const AActor* Player) const
{
	const UAuraFishingComponent* FishingComponent = IFishingActorInterface::GetFishingComponent(Player);
	if (!FishingComponent)
	{
		UE_LOG(LogAura, Warning, TEXT("[%s] No fishing component on actor: %s"), *GetName(), *Player->GetName())
		return TArray<FWeightedFish>();
	}
	TArray<FWeightedFish> AvailableFish;
	FGameplayTagContainer PlayerFishTags = FishingComponent->GetFishingTags();
	FGameplayTagContainer CombinedFishTags;
	CombinedFishTags.AppendTags(PoolTags);
	CombinedFishTags.AppendTags(PlayerFishTags);
	if (const UAuraGameDataSubsystem* GameMode = UAuraGameDataSubsystem::Get(this))
	{
		for (const FFishConfig& FishConfig : FishConfigs)
		{
			const FAuraFishDefinition& FishDefinition = GameMode->GetFishInfo()->GetFishDefinitionByFishType(
				FishConfig.FishType
			);
			if (CombinedFishTags.HasAll(FishDefinition.Tags))
			{
				const float BaseRarityMultiplier = GameMode->GetFishInfo()->GetFishRarityMultiplierByRarity(
					FishDefinition.Rarity
				);
				const float PlayerRarityMultiplier = FishingComponent->GetRarityMultiplier(FishDefinition.Rarity);
				FWeightedFish WeightedFish;
				WeightedFish.FishType = FishConfig.FishType;
				WeightedFish.Weight = 100.f * (FishConfig.RarityMultiplier * BaseRarityMultiplier *
					PlayerRarityMultiplier);
				if (bDebug)
				{
					UE_LOG(
						LogAura,
						Warning,
						TEXT("[%s]: Rarity multiplier for fish [%s] | [%f] * [%f]"),
						*GetName(),
						*FishConfig.FishType.ToString(),
						BaseRarityMultiplier,
						PlayerRarityMultiplier
					);
				}
				if (WeightedFish.Weight > 0)
				{
					AvailableFish.Add(WeightedFish);
				}
			}
		}
	}
	return AvailableFish;
}

FGameplayTag AAuraFishingPOI::PickFishType(const TArray<FWeightedFish>& AvailableFish) const
{
	float TotalWeight = 0.f;
	for (const auto& [FishType, Weight] : AvailableFish)
	{
		TotalWeight += Weight;
	}
	TArray<FWeightedFish> FishByWeight = AvailableFish;
	FishByWeight.Sort(
		[](const FWeightedFish& A, const FWeightedFish& B)
		{
			return A.Weight < B.Weight;
		}
	);
	const float Roll = FMath::RandRange(0.f, TotalWeight);
	if (bDebug)
	{
		UE_LOG(LogAura, Warning, TEXT("[%s]: Player rolled [%f] for fish."), *GetName(), Roll);
	}
	float CumulativeWeight = 0.f;
	for (const auto& [FishType, Weight] : FishByWeight)
	{
		CumulativeWeight += Weight;
		if (bDebug)
		{
			UE_LOG(
				LogAura,
				Warning,
				TEXT("[%s]: Checking roll [%f] against weight [%f] (combined: [%f]"),
				*GetName(),
				Roll,
				Weight,
				CumulativeWeight
			);
		}
		if (Roll < CumulativeWeight)
		{
			if (bDebug)
			{
				UE_LOG(LogAura, Warning, TEXT("[%s]: Chose a fish! [%s]"), *GetName(), *FishType.ToString());
			}
			return FishType;
		}
	}
	return FGameplayTag::EmptyTag;
}

void AAuraFishingPOI::HandleInteract_Implementation(AActor* Player)
{
	Super::HandleInteract_Implementation(Player);
	if (bIsBusy)
	{
		return;
	}
	StartFishing(Player);
}


void AAuraFishingPOI::StartFishing_Implementation(AActor* Player)
{
	bIsBusy = true;
	HideWidgets();
	if (UAuraFishingComponent* FishingComponent = IFishingActorInterface::GetFishingComponent(Player))
	{
		FishingComponent->SetupForFishing(FishingTarget->GetChildActor()->GetActorLocation());
	}
}

void AAuraFishingPOI::CancelFishing_Implementation(AActor* Player)
{
	EndFishing(Player);
}

void AAuraFishingPOI::TriggerEquipFishingRod_Implementation(AActor* Player)
{
	if (UAuraFishingComponent* FishingComponent = IFishingActorInterface::GetFishingComponent(Player))
	{
		FishingComponent->EquipFishingRod();
	}
	else
	{
		CancelFishing(Player);
	}
}


void AAuraFishingPOI::TriggerFishingCast_Implementation(AActor* Player)
{
	if (UAuraFishingComponent* FishingComponent = IFishingActorInterface::GetFishingComponent(Player))
	{
		FishingComponent->CastFishingRod();
	}
	else
	{
		CancelFishing(Player);
	}
}


void AAuraFishingPOI::TriggerSplashEffect_Implementation(const FSplashEffect SplashEffect)
{
	SplashSFXComponent->SetBoolParameter(FName("Loop"), SplashEffect.LoopSound);
	FLoadSoftObjectPathAsyncDelegate SFXLoadedDelegate;
	SFXLoadedDelegate.BindLambda([this, SplashEffect](FSoftObjectPath ObjectPath, UObject* Loaded)
	{
		SplashSFXComponent->SetSound(SplashEffect.SplashSound.Get());
		SplashSFXComponent->Play();
	});
	FLoadSoftObjectPathAsyncDelegate NiagaraLoadedDelegate;
	NiagaraLoadedDelegate.BindLambda([this, SplashEffect](FSoftObjectPath ObjectPath, UObject* Loaded)
	{
		SplashFXComponent->SetAsset(SplashEffect.SplashSystem.Get());
		SplashFXComponent->Activate();
	});
	if (AAuraWaterRipple* WaterRipple = Cast<AAuraWaterRipple>(Ripple->GetChildActor()))
	{
		WaterRipple->PlayRipple(SplashEffect.LoopSound);
	}
	SplashEffect.SplashSound.LoadAsync(SFXLoadedDelegate);
	SplashEffect.SplashSystem.LoadAsync(NiagaraLoadedDelegate);
}

void AAuraFishingPOI::TriggerBobSplashEffect()
{
	TriggerSplashEffect(BobSplashEffect);
}

void AAuraFishingPOI::TriggerBiteSplashEffect()
{
	TriggerSplashEffect(BiteSplashEffect);
}

void AAuraFishingPOI::StopSplashEffect_Implementation()
{
	if (AAuraWaterRipple* WaterRipple = Cast<AAuraWaterRipple>(Ripple->GetChildActor()))
	{
		WaterRipple->StopRipple();
	}
	SplashSFXComponent->Stop();
	SplashFXComponent->Deactivate();
}


void AAuraFishingPOI::TriggerFishInterested_Implementation(AActor* Player, UAuraFishingBlueprintNode* FishingTask)
{
	TArray<FWeightedFish> AvailableFish = GetCurrentlyAvailableFish(Player);
	const FGameplayTag& ChosenFishType = PickFishType(AvailableFish);
	if (ChosenFishType.IsValid() && IsValid(FishingTask))
	{
		FishingTask->LureAndWaitForABite(ChosenFishType);
	}
	else
	{
		CancelFishing(Player);
	}
}


void AAuraFishingPOI::TriggerFishBite_Implementation(AActor* Player)
{
	TriggerBiteSplashEffect();
	ShowCatchWidget(Player);
}

void AAuraFishingPOI::TriggerFishReel_Implementation(AActor* Player, UAuraFishingBlueprintNode* FishingTask)
{
	if (IsValid(Player))
	{
		OnFishCatchWidgetHide.Broadcast();
	}
	else
	{
		CancelFishing(Player);
	}
}

void AAuraFishingPOI::TriggerFishCatch_Implementation(AActor* Player, UAuraFishingBlueprintNode* FishingTask)
{
	if (IsValid(Player) && IsValid(FishingTask))
	{
		FishingTask->Catch();
	}
}

void AAuraFishingPOI::TriggerFishCaught_Implementation(AActor* Player, const UAuraFishCatch* FishCatch)
{
	StopSplashEffect();
	ShowSuccessWidget(Player, FishCatch);
}

void AAuraFishingPOI::TriggerFishFled_Implementation(AActor* Player)
{
	OnFishCatchWidgetHide.Broadcast();
	StopSplashEffect();
	ShowFailureWidget(Player);
}

void AAuraFishingPOI::ShowSuccessWidget_Implementation(AActor* Player, const UAuraFishCatch* FishCatch)
{
	UAuraFishingMessageWidget* SuccessWidget = CreateSuccessMessageWidget(Player);
	IFishingSuccessWidgetInterface::SetFishingCatchInfo(SuccessWidget, FishCatch);
	SuccessWidget->OnContinue.AddDynamic(this, &AAuraFishingPOI::StartFishing);
	SuccessWidget->OnCancel.AddDynamic(this, &AAuraFishingPOI::CancelFishing);
	// FishCatch needs to a class instead of a struct.
	SuccessWidget->OnHidden.AddDynamic(FishCatch, &UAuraFishCatch::AddToPlayerInventory);
	SuccessWidget->AddToViewport();
	SuccessWidget->Show();
}

UAuraFishingMessageWidget* AAuraFishingPOI::CreateSuccessMessageWidget_Implementation(AActor* Player)
{
	return CreateWidget<UAuraFishingMessageWidget>(UAuraWidgetFunctionLibrary::GetPlayerController(Player), SuccessWidgetClass, FName("Fishing Success"));
}

void AAuraFishingPOI::ShowFailureWidget_Implementation(AActor* Player)
{
	UAuraFishingMessageWidget* SuccessWidget = CreateFailureMessageWidget(Player);
	SuccessWidget->OnContinue.AddDynamic(this, &AAuraFishingPOI::StartFishing);
	SuccessWidget->OnCancel.AddDynamic(this, &AAuraFishingPOI::CancelFishing);
	SuccessWidget->AddToViewport();
	SuccessWidget->Show();
}

UAuraFishingMessageWidget* AAuraFishingPOI::CreateFailureMessageWidget_Implementation(AActor* Player)
{
	return CreateWidget<UAuraFishingMessageWidget>(UAuraWidgetFunctionLibrary::GetPlayerController(Player), FailureWidgetClass, FName("Fishing Failure"));
}

void AAuraFishingPOI::ShowCatchWidget_Implementation(AActor* Player)
{
	UAuraInteractionWidget* CatchWidget = CreateCatchWidget(Player);
	IInteractionWidgetInterface::Execute_SetActionText(CatchWidget, CatchWidgetText);
	IInteractionWidgetInterface::Execute_SetStartHidden(CatchWidget, true);
	IInteractionWidgetInterface::Execute_SetAutoDestroyOnHide(CatchWidget, true);
	CatchWidget->AddToViewport();
	CatchWidget->SetPositionInViewport(GetScreenPositionOfFishingTarget(Player));
	OnFishCatchWidgetHide.AddLambda([CatchWidget]()
	{
		if (IsValid(CatchWidget))
		{
			IInteractionWidgetInterface::Execute_Hide(CatchWidget);
		}
	});
	IInteractionWidgetInterface::Execute_Show(CatchWidget);
}

UAuraInteractionWidget* AAuraFishingPOI::CreateCatchWidget_Implementation(AActor* Player)
{
	return CreateWidget<UAuraInteractionWidget>(UAuraWidgetFunctionLibrary::GetPlayerController(Player), CatchWidgetClass, FName("Catch"));
}

FVector2D AAuraFishingPOI::GetScreenPositionOfFishingTarget(AActor* Player) const
{
	FVector2D ScreenLocation;
	if (const APawn* PlayerPawn = Cast<APawn>(Player))
	{
		if (const APlayerController* PlayerController = PlayerPawn->GetLocalViewingPlayerController())
		{
			PlayerController->ProjectWorldLocationToScreen(FishingTarget->GetChildActor()->GetActorLocation(), ScreenLocation);
			ScreenLocation.X = ScreenLocation.X - 256;
			ScreenLocation.Y = ScreenLocation.Y - 32;
		}
	}
	return ScreenLocation;
}

FAuraGoFishingParams AAuraFishingPOI::MakeGoFishingParams() const
{
	FAuraGoFishingParams GoFishingParams;
	GoFishingParams.FishingTarget = FishingTarget->GetChildActor();
	GoFishingParams.PlayerTarget = PlayerTarget->GetChildActor();
	GoFishingParams.CameraTarget = CameraTarget->GetChildActor();
	GoFishingParams.CameraMovementCurve = CameraMovementCurve;
	GoFishingParams.BiteToFleeTime = BiteToFleeTime;
	GoFishingParams.LureToBiteTime = LureToBiteTime;
	GoFishingParams.TimeToLure = InterestToLureTime;
	GoFishingParams.ReelEffectClass = ReelAvailableEffectClass;
	return GoFishingParams;
}

void AAuraFishingPOI::ShowWidgets()
{
	POIWidget->SetVisibility(true);
	InteractionWidget->SetVisibility(true);
	PreconditionWidget->SetVisibility(true);
}

void AAuraFishingPOI::HideWidgets()
{
	POIWidget->SetVisibility(false);
	InteractionWidget->SetVisibility(false);
	PreconditionWidget->SetVisibility(false);
}

void AAuraFishingPOI::EndFishing_Implementation(AActor* Player)
{
	StopSplashEffect();
	if (UAuraFishingComponent* FishingComponent = IFishingActorInterface::GetFishingComponent(Player))
	{
		FishingComponent->EndFishing();
	}
	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Player))
	{
		AbilitySystemComponent->TryActivateAbilitiesByTag(FAuraGameplayTags::Get().Abilities_Other_Cancel.GetSingleTagContainer());
	}
	IPlayerInterface::Execute_ReturnCamera(Player, CameraMovementCurve);
	bIsBusy = false;
}

void AAuraFishingPOI::FinishFishingRound_Implementation(AActor* Player)
{
	// nothing to do here right now?
}
