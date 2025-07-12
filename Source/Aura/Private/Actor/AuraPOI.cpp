// Copyright Alien Shores


#include "Actor/AuraPOI.h"

#include "Aura/Aura.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Item/Component/AuraEffectComponent.h"
#include "UI/Widget/InteractionWidgetInterface.h"


AAuraPOI::AAuraPOI()
{
	PrimaryActorTick.bCanEverTick = false;
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SphereComponent->SetCollisionObjectType(ECC_WorldStatic);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetGenerateOverlapEvents(true);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SetRootComponent(SphereComponent);
	POIWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("POIWidget"));
	POIWidget->SetupAttachment(GetRootComponent());
	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidget->SetupAttachment(GetRootComponent());
}

void AAuraPOI::BeginPlay()
{
	Super::BeginPlay();
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AAuraPOI::OnSphereBeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AAuraPOI::OnSphereEndOverlap);
	IInteractionWidgetInterface::Hide(InteractionWidget->GetWidget(), false);
}

void AAuraPOI::OnSphereBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool BFromSweep,
	const FHitResult& SweepResult
)
{
	// if player, show interaction widget
	if (IsPlayerActor(OtherActor) && IsPreconditionMet(OtherActor))
	{
		EffectComponent->OnOverlap(OtherActor);
		ShowInteractWithPOIAvailable(OtherActor);
	}
	else
	{
		ShowInteractWithPOINotAvailable(OtherActor);
	}
}

void AAuraPOI::OnSphereEndOverlap(
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
	}
}

void AAuraPOI::ShowInteractWithPOIAvailable_Implementation(AActor* Player) const
{
	IInteractionWidgetInterface::Show(InteractionWidget->GetWidget());
}

void AAuraPOI::ShowInteractWithPOINotAvailable_Implementation(AActor* Player) const
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

void AAuraPOI::OnInteractionEnd_Implementation(AActor* Player, const bool bIsCancelled)
{
	// TODO ?
}

bool AAuraPOI::IsPlayerActor(const AActor* Actor) const
{
	return IsValid(Actor) && Actor->ActorHasTag(TAG_PLAYER);
}
