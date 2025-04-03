// Copyright Alien Shores


#include "Checkpoint/Checkpoint.h"

#include "Aura/Aura.h"
#include "Components/SphereComponent.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/PlayerInterface.h"

ACheckpoint::ACheckpoint(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	CheckpointMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CheckpointMesh"));
	CheckpointMesh->SetupAttachment(GetRootComponent());
	CheckpointMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CheckpointMesh->SetCollisionResponseToAllChannels(ECR_Block);
	CheckpointMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_TAN);
	CheckpointMesh->MarkRenderStateDirty();
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(CheckpointMesh);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	MoveToComponent = CreateDefaultSubobject<USceneComponent>(TEXT("MoveToComponent"));
	MoveToComponent->SetupAttachment(GetRootComponent());
}

void ACheckpoint::LoadActor_Implementation()
{
	if (bHasBeenActivated && HasActorBegunPlay())
	{
		HandleGlowEffects();
	}
}

void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnSphereOverlap);
	if (bHasBeenActivated)
	{
		HandleGlowEffects();
	}
}

void ACheckpoint::OnSphereOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (IsValid(OtherActor) && OtherActor->Implements<UPlayerInterface>())
	{
		bHasBeenActivated = true;
		HandleGlowEffects();
		AAuraGameModeBase* GameMode = AAuraGameModeBase::GetAuraGameMode(this);
		IPlayerInterface::Execute_SaveProgress(OtherActor, PlayerStartTag);
		GameMode->SaveWorldState(GetWorld());
	}
}

void ACheckpoint::HandleGlowEffects()
{
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(
		CheckpointMesh->GetMaterial(0),
		this
	);
	CheckpointMesh->SetMaterial(0, DynamicMaterialInstance);
	CheckpointReached(DynamicMaterialInstance);
}

void ACheckpoint::HighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(true);
}

void ACheckpoint::UnHighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(false);
}

bool ACheckpoint::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	OutDestination = MoveToComponent->GetComponentLocation();
	return true;
}
