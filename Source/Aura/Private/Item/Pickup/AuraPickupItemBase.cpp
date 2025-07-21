// Copyright Alien Shores


#include "Item/Pickup/AuraPickupItemBase.h"

#include "NiagaraFunctionLibrary.h"
#include "Aura/Aura.h"
#include "Components/SphereComponent.h"
#include "Item/Component/SinusoidalMovementComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AAuraPickupItemBase::AAuraPickupItemBase()
{
	PrimaryActorTick.bCanEverTick = false;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SphereComponent->SetupAttachment(GetRootComponent());
	SphereComponent->SetCollisionObjectType(ECC_WorldDynamic);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
	SphereComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	SphereComponent->SetCollisionResponseToChannel(ECC_Projectile, ECR_Ignore);
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	MeshComponent->SetupAttachment(SphereComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetRenderCustomDepth(false);
	SinusoidalMovementComponent = CreateDefaultSubobject<USinusoidalMovementComponent>(TEXT("Sinusoidal Movement Component"));
}

// Called when the game starts or when spawned
void AAuraPickupItemBase::BeginPlay()
{
	Super::BeginPlay();
	MeshComponent->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	PlaySpawnEffect();
}

void AAuraPickupItemBase::HighlightActor_Implementation()
{
	MeshComponent->SetRenderCustomDepth(true);
}

void AAuraPickupItemBase::UnHighlightActor_Implementation()
{
	MeshComponent->SetRenderCustomDepth(false);
}

void AAuraPickupItemBase::PlaySpawnEffect_Implementation()
{
	if (SpawnSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SpawnSound, GetActorLocation());
	}
	if (SpawnEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, SpawnEffect, GetActorLocation(), GetActorRotation());
	}
}

void AAuraPickupItemBase::PlayPickupEffect_Implementation(AActor* PickupActor, const bool bAutoDestroy)
{
	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			PickupActor,
			PickupSound,
			PickupActor->GetActorLocation());
	}
}
