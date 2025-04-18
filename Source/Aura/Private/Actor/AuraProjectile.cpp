// Copyright Alien Shores


#include "Actor/AuraProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/Aura.h"
#include "Components/AudioComponent.h"
#include "Interaction/CombatInterface.h"
#include "Net/UnrealNetwork.h"
#include "Utils/TagUtils.h"

AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	// DEVNOTE: Ensures that the state is replicated from server -> client
	bReplicates = true;
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

void AAuraProjectile::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAuraProjectile, DamageEffectParams);
}

UProjectileMovementComponent* AAuraProjectile::GetProjectileMovementComponent() const
{
	return ProjectileMovement;
}

void AAuraProjectile::OnTargetDead(AActor* DeadActor)
{
	PlayImpactEffect();
	if (HasAuthority())
	{
		Destroy();
	}
}

FVector AAuraProjectile::GetImpactDirection(const AActor* HitActor) const
{
	if (IsValid(HitActor))
	{
		return HitActor->GetActorLocation() - GetActorLocation();
	}
	return FVector::ZeroVector;
}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetReplicateMovement(true);
	SetLifeSpan(LifeSpan);
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);
	if (TravelSound)
	{
		TravelSoundComponent = UGameplayStatics::SpawnSoundAttached(TravelSound, GetRootComponent());
	}
	if (TravelSoundComponent)
	{
		TravelSoundComponent->SetSound(TravelSound);
		TravelSoundComponent->Play();
	}
}

void AAuraProjectile::OnSphereOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!IsValidOverlap(OtherActor))
	{
		return;
	}
	PlayImpactEffect();
	if (HasAuthority())
	{
		if (UAbilitySystemComponent* OtherAbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
			OtherActor
		))
		{
			DamageEffectParams.DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			if (FMath::RandRange(1, 100) < DamageEffectParams.KnockbackChance)
			{
				FRotator Rotation = GetActorRotation();
				Rotation.Pitch = 45.f;
				DamageEffectParams.KnockbackForce = Rotation.Vector() * DamageEffectParams.KnockbackForceMagnitude;
			}
			DamageEffectParams.TargetAbilitySystemComponent = OtherAbilitySystem;
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
		Destroy();
	}
}

bool AAuraProjectile::IsValidOverlap(const AActor* OtherActor) const
{
	const AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	if (SourceAvatarActor == OtherActor)
	{
		return false;
	}
	if (TagUtils::HasAnyTag(OtherActor, ICombatInterface::GetTargetTagsToIgnore(SourceAvatarActor)))
	{
		return false;
	}
	return true;
}

void AAuraProjectile::PlayImpactEffect()
{
	if (bHit)
	{
		// Only play impact effect once
		return;
	}
	// DEVNOTE - this could alternatively be done as a GameplayCue_Burst.
	// This has implications on the number of RPCs, so we should use ExecuteGameplayCue_NonReplicated
	// if we choose to go that route in the future.
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			ImpactSound,
			GetActorLocation(),
			FRotator::ZeroRotator
		);
	}
	if (ImpactEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			ImpactEffect,
			GetActorLocation()
		);
	}
	if (TravelSoundComponent)
	{
		TravelSoundComponent->Stop();
		TravelSoundComponent->DestroyComponent();
	}
	bHit = true;
}

void AAuraProjectile::Destroyed()
{
	if (!HasAuthority())
	{
		PlayImpactEffect();
	}
	Super::Destroyed();
}
