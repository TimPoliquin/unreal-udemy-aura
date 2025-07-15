// Copyright Alien Shores


#include "Item/Pickup/AuraPickupItemInstant.h"

#include "Components/SphereComponent.h"
#include "Item/Component/AuraEffectComponent.h"


// Sets default values
AAuraPickupItemInstant::AAuraPickupItemInstant()
{
	PrimaryActorTick.bCanEverTick = false;
	EffectComponent = CreateDefaultSubobject<UAuraEffectComponent>(TEXT("Effect Component"));
}

// Called when the game starts or when spawned
void AAuraPickupItemInstant::BeginPlay()
{
	Super::BeginPlay();
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AAuraPickupItemInstant::OnBeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AAuraPickupItemInstant::OnEndOverlap);
}

void AAuraPickupItemInstant::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                            const FHitResult& SweepResult)
{
	PlayPickupEffect(OtherActor, true);
	EffectComponent->OnOverlap(OtherActor);
}

void AAuraPickupItemInstant::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	EffectComponent->OnEndOverlap(OtherActor);
}
