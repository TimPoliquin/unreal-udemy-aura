// Copyright Alien Shores


#include "Actor/Recovery/AuraRecoveryActor.h"

#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Item/Component/AuraEffectComponent.h"


AAuraRecoveryActor::AAuraRecoveryActor()
{
	PrimaryActorTick.bCanEverTick = true;
	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComponent"));
	SetRootComponent(CollisionComponent);
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(GetRootComponent());
	NiagaraEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraEffectComponent"));
	NiagaraEffectComponent->SetupAttachment(GetRootComponent());
	RecoveryEffectComponent = CreateDefaultSubobject<UAuraEffectComponent>(TEXT("Recovery Effect Component"));
}

void AAuraRecoveryActor::BeginPlay()
{
	Super::BeginPlay();
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AAuraRecoveryActor::OnBeginOverlap);
	CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &AAuraRecoveryActor::OnEndOverlap);
	PlayEnvironmentalSound();
}

// Called every frame
void AAuraRecoveryActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!OverlappingActors.IsEmpty())
	{
		if (HasRecoveryTime())
		{
			ConsumedRecoveryTime = FMath::Clamp(ConsumedRecoveryTime + DeltaTime, 0, TotalRecoveryTime);
			ClearRecoveredActors();
		}
		else
		{
			ExpireRecovery();
		}
	}
}

bool AAuraRecoveryActor::HasRecoveryTime() const
{
	return ConsumedRecoveryTime < TotalRecoveryTime;
}

void AAuraRecoveryActor::PlayEnvironmentalSound_Implementation()
{
	AudioComponent->Stop();
	AudioComponent->SetSound(EnvironmentalSound);
	AudioComponent->Play();
}

bool AAuraRecoveryActor::CheckPreRequisites_Implementation(AActor* PickupActor) const
{
	return RecoveryEffectComponent->CheckPreRequisites(PickupActor);
}


void AAuraRecoveryActor::ExpireRecovery_Implementation()
{
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (NiagaraEffectComponent)
	{
		NiagaraEffectComponent->Deactivate();
	}
	if (AudioComponent)
	{
		AudioComponent->Stop();
	}
}

void AAuraRecoveryActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                        const FHitResult& SweepResult)
{
	if (HasRecoveryTime() && CheckPreRequisites(OtherActor))
	{
		if (OverlappingActors.IsEmpty())
		{
			AudioComponent->Stop();
			AudioComponent->SetSound(RecoverySound);
			AudioComponent->Play();
		}
		OverlappingActors.Add(OtherActor);
		RecoveryEffectComponent->OnOverlap(OtherActor);
	}
}


void AAuraRecoveryActor::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	EndRecovery(OtherActor);
}

void AAuraRecoveryActor::ClearRecoveredActors()
{
	TArray<AActor*> RecoveredActors;
	for (AActor* AffectedActor : OverlappingActors)
	{
		if (!CheckPreRequisites(AffectedActor))
		{
			RecoveredActors.Add(AffectedActor);
		}
	}
	for (AActor* RecoveredActor : RecoveredActors)
	{
		EndRecovery(RecoveredActor);
	}
}

void AAuraRecoveryActor::EndRecovery(AActor* OtherActor)
{
	if (RecoveryEffectComponent->CheckPreRequisites(OtherActor) && OverlappingActors.Contains(OtherActor))
	{
		OverlappingActors.Remove(OtherActor);
		RecoveryEffectComponent->OnEndOverlap(OtherActor);
		if (AudioComponent && HasRecoveryTime() && OverlappingActors.IsEmpty())
		{
			PlayEnvironmentalSound();
		}
	}
}
