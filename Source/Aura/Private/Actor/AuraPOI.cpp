﻿// Copyright Alien Shores


#include "Actor/AuraPOI.h"

#include "Aura/Aura.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
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
	ApplyToTags.AddUnique(TAG_PLAYER);
	ApplyToTags.Remove(TAG_ENEMY);
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
	if (IsPlayerActor(OtherActor))
	{
		OnOverlap(OtherActor);
		IInteractionWidgetInterface::Show(InteractionWidget->GetWidget());
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
		OnEndOverlap(OtherActor);
		IInteractionWidgetInterface::Hide(InteractionWidget->GetWidget());
	}
}

bool AAuraPOI::IsPlayerActor(const AActor* Actor) const
{
	return IsValid(Actor) && Actor->ActorHasTag(TAG_PLAYER);
}
