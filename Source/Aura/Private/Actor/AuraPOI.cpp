// Copyright Alien Shores


#include "Actor/AuraPOI.h"

#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Item/Component/AuraEffectComponent.h"
#include "UI/Widget/InteractionWidgetInterface.h"


AAuraPOI::AAuraPOI()
{
	PrimaryActorTick.bCanEverTick = false;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
	OverlapDetectionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Overlap Detection Component"));
	OverlapDetectionComponent->SetCollisionObjectType(ECC_WorldStatic);
	OverlapDetectionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapDetectionComponent->SetGenerateOverlapEvents(true);
	OverlapDetectionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapDetectionComponent->SetupAttachment(GetRootComponent());
	POIWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("POIWidget"));
	POIWidget->SetupAttachment(GetRootComponent());
	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidget->SetupAttachment(GetRootComponent());
	PreconditionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PreconditionWidget"));
	PreconditionWidget->SetupAttachment(GetRootComponent());
	TArray<FGameplayEffectConfig> EffectConfigs;
	EffectConfigs.Add(FGameplayEffectConfig(nullptr, EEffectApplicationPolicy::ApplyOnOverlap, EEffectRemovalPolicy::RemoveOnEndOverlap));
	TArray<FName> ApplyToTags;
	ApplyToTags.Add(TAG_PLAYER);
	EffectComponent->SetDefaults(EffectConfigs, false, ApplyToTags);
}

void AAuraPOI::BeginPlay()
{
	Super::BeginPlay();
	OverlapDetectionComponent->OnComponentBeginOverlap.AddDynamic(this, &AAuraPOI::OnBeginOverlap);
	OverlapDetectionComponent->OnComponentEndOverlap.AddDynamic(this, &AAuraPOI::OnEndOverlap);
	IInteractionWidgetInterface::Hide(InteractionWidget->GetWidget(), false);
	IInteractionWidgetInterface::Hide(PreconditionWidget->GetWidget(), false);
}

void AAuraPOI::OnBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool BFromSweep,
	const FHitResult& SweepResult
)
{
	if (bDisabled)
	{
		return;
	}
	if (!IsPlayerActor(OtherActor))
	{
		return;
	}
	// if player, show interaction widget
	if (IsPreconditionMet(OtherActor))
	{
		EffectComponent->OnOverlap(OtherActor);
		ShowInteractWithPOIAvailable(OtherActor);
	}
	else
	{
		ShowPreconditionWidget(OtherActor);
	}
}

void AAuraPOI::OnEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
{
	// if player, hide interaction widget
	if (IsPlayerActor(OtherActor))
	{
		EffectComponent->OnEndOverlap(OtherActor);
		IInteractionWidgetInterface::Hide(InteractionWidget->GetWidget());
		IInteractionWidgetInterface::Hide(PreconditionWidget->GetWidget());
	}
}

void AAuraPOI::ShowInteractWithPOIAvailable_Implementation(AActor* Player) const
{
	IInteractionWidgetInterface::Show(InteractionWidget->GetWidget());
}

void AAuraPOI::ShowPreconditionWidget_Implementation(AActor* Player) const
{
	if (PreconditionWidget && PreconditionWidget->GetWidget())
	{
		IInteractionWidgetInterface::Show(PreconditionWidget->GetWidget());
	}
}

bool AAuraPOI::IsPreconditionMet_Implementation(AActor* Player) const
{
	return true;
}

bool AAuraPOI::OnInteract_Implementation(AActor* Player)
{
	if (IsPreconditionMet(Player))
	{
		return HandleInteract(Player);
	}
	return false;
}

bool AAuraPOI::HandleInteract_Implementation(AActor* Player)
{
	return true;
}

void AAuraPOI::OnInteractionEnd_Implementation(AActor* Player, const bool bIsCancelled)
{
	// TODO ?
}

void AAuraPOI::DisablePOI()
{
	bDisabled = true;
	OverlapDetectionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (POIWidget && POIWidget->GetWidget())
	{
		POIWidget->GetWidget()->SetVisibility(ESlateVisibility::Hidden);
	}
}

bool AAuraPOI::IsPlayerActor(const AActor* Actor) const
{
	return IsValid(Actor) && Actor->ActorHasTag(TAG_PLAYER);
}
