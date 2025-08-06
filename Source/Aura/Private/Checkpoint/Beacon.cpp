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
		ActivateBeacon();
	}
}


EBeaconValidationState ABeacon::ValidateBeaconActivation(const ABeacon* CurrentBeacon, const TArray<ABeacon*>& Beacons, const bool IsOrdered)
{
	EBeaconValidationState ValidationState = EBeaconValidationState::Complete;
	bool CurrentBeaconFound = false;
	for (const ABeacon* Beacon : Beacons)
	{
		if (Beacon == CurrentBeacon)
		{
			CurrentBeaconFound = true;
		}
		if (!Beacon->IsBeaconActivated())
		{
			if (IsOrdered && !CurrentBeaconFound)
			{
				ValidationState = EBeaconValidationState::OutOfOrder;
			}
			else
			{
				ValidationState = EBeaconValidationState::Incomplete;
			}
			break;
		}
	}
	return ValidationState;
}

void ABeacon::BeginPlay()
{
	Super::BeginPlay();
	OriginalMaterial = BeaconMesh->GetMaterial(0);
	DynamicMaterialInstance = UMaterialInstanceDynamic::Create(
		OriginalMaterial,
		this
	);
	if (bHasBeenActivated)
	{
		ActivateBeacon();
	}
}

void ABeacon::ActivateBeacon()
{
	if (bHasBeenActivated)
	{
		return;
	}
	bHasBeenActivated = true;
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BeaconMesh->SetMaterial(0, DynamicMaterialInstance);
	CheckpointReached(DynamicMaterialInstance);
}

bool ABeacon::IsBeaconActivated() const
{
	return bHasBeenActivated;
}

void ABeacon::ResetBeacon_Implementation()
{
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BeaconMesh->SetMaterial(0, OriginalMaterial);
	bHasBeenActivated = false;
}
