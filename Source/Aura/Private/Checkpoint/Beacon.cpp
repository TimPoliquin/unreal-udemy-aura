// Copyright Alien Shores


#include "Checkpoint/Beacon.h"

#include "Aura/Aura.h"
#include "Components/SphereComponent.h"


// Sets default values
ABeacon::ABeacon()
{
	PrimaryActorTick.bCanEverTick = false;
	BeaconMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BeaconMesh"));
	BeaconMesh->SetupAttachment(GetRootComponent());
	BeaconMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BeaconMesh->SetCollisionResponseToAllChannels(ECR_Block);
	BeaconMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_TAN);
	BeaconMesh->MarkRenderStateDirty();
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(BeaconMesh);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ABeacon::LoadActor_Implementation()
{
	if (bHasBeenActivated && HasActorBegunPlay())
	{
		HandleGlowEffects();
	}
}

void ABeacon::BeginPlay()
{
	Super::BeginPlay();
	if (bHasBeenActivated)
	{
		HandleGlowEffects();
	}
}

void ABeacon::HandleGlowEffects()
{
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(
		BeaconMesh->GetMaterial(0),
		this
	);
	BeaconMesh->SetMaterial(0, DynamicMaterialInstance);
	CheckpointReached(DynamicMaterialInstance);
}
