// Copyright Alien Shores


#include "Item/AuraFishingRod.h"

#include "AuraFishingBob.h"
#include "CableComponent.h"
#include "Actor/AuraActorBlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAuraFishingRod::AAuraFishingRod()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CableComponent = CreateDefaultSubobject<UCableComponent>(TEXT("FishingLine"));
	CableComponent->bAttachStart = true;
	CableComponent->SetupAttachment(MeshComponent, RodTipSocket);
}

FVector AAuraFishingRod::GetRodTipLocation() const
{
	return GetMesh()->GetSocketLocation(RodTipSocket);
}

FRotator AAuraFishingRod::GetRodTipRotation() const
{
	return GetMesh()->GetSocketRotation(RodTipSocket);
}


void AAuraFishingRod::BeginPlay()
{
	Super::BeginPlay();
	BobActor = CreateFishingBob();
	CableComponent->SetAttachEndTo(BobActor, BobActor->GetMeshComponentName());
	CableComponent->SetVisibility(false);
	CableComponent->Deactivate();
}

AAuraFishingBob* AAuraFishingRod::CreateFishingBob()
{
	FTransform Transform;
	Transform.SetLocation(GetRodTipLocation());
	Transform.SetRotation(GetRodTipRotation().Quaternion());
	AAuraFishingBob* FishingBob = GetWorld()->SpawnActorDeferred<AAuraFishingBob>(
		FishingBobClass,
		Transform,
		this,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	FishingBob->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale, RodTipSocket);
	FishingBob->OnFishingStateChanged.AddDynamic(this, &AAuraFishingRod::OnFishingBobStateChanged);
	UGameplayStatics::FinishSpawningActor(FishingBob, Transform);
	UAuraActorBlueprintFunctionLibrary::DisableActor(FishingBob);
	return FishingBob;
}

void AAuraFishingRod::Cast(const FVector& Destination)
{
	CableComponent->SetVisibility(true);
	CableComponent->Activate();
	BobActor->SetActorLocation(GetRodTipLocation());
	BobActor->Launch(Destination);
}

void AAuraFishingRod::Return()
{
	CableComponent->SetVisibility(false);
	CableComponent->Deactivate();
	if (BobActor)
	{
		BobActor->Return(this, RodTipSocket);
	}
}

void AAuraFishingRod::UnEquip(AActor* InOwner)
{
	Super::UnEquip(InOwner);
	if (IsValid(BobActor))
	{
		BobActor->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale, RodTipSocket);
		UAuraActorBlueprintFunctionLibrary::DisableActor(BobActor);
		CableComponent->SetVisibility(false);
		CableComponent->Deactivate();
	}
}

AAuraFishingBob* AAuraFishingRod::GetFishingBob() const
{
	return BobActor;
}

void AAuraFishingRod::OnFishingBobStateChanged(EFishingBobState FishingState)
{
	switch (FishingState)
	{
	case EFishingBobState::Bobbing:
		OnFishingBobLandedDelegate.Broadcast();
		break;
	default:
		// DO NOTHING
		break;
	}
}
