// Copyright Alien Shores


#include "Item/Pickup/AuraPickupItemBase.h"

#include "Components/SphereComponent.h"
#include "Item/Component/SinusoidalMovementComponent.h"


// Sets default values
AAuraPickupItemBase::AAuraPickupItemBase()
{
	PrimaryActorTick.bCanEverTick = false;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SphereComponent->SetupAttachment(GetRootComponent());
	SphereComponent->SetCollisionObjectType(ECC_WorldDynamic);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	MeshComponent->SetupAttachment(SphereComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SinusoidalMovementComponent = CreateDefaultSubobject<USinusoidalMovementComponent>(TEXT("Sinusoidal Movement Component"));
}

// Called when the game starts or when spawned
void AAuraPickupItemBase::BeginPlay()
{
	Super::BeginPlay();
}
