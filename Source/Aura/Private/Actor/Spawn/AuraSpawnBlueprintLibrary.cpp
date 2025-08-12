// Copyright Alien Shores


#include "Actor/Spawn/AuraSpawnBlueprintLibrary.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/AuraLogChannels.h"
#include "Kismet/KismetMathLibrary.h"

TArray<FTransform> UAuraSpawnBlueprintLibrary::GenerateSpawnLocations(const FVector& Center, const float Radius, const int32 NumSpawns)
{
	TArray<FTransform> SpawnTransforms;
	TArray<FRotator> Rotators = UAuraAbilitySystemLibrary::EvenlySpacedRotators(FVector::ForwardVector, FVector::UpVector, 360, NumSpawns);
	for (FRotator Rotator : Rotators)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(UKismetMathLibrary::GetForwardVector(Rotator) * Radius + Center);
		SpawnTransform.SetRotation(Rotator.Quaternion());
		SpawnTransforms.Add(SpawnTransform);
	}
	return SpawnTransforms;
}

FAuraSpawnParams UAuraSpawnBlueprintLibrary::Pop(TArray<FAuraSpawnParams>& SpawnParams)
{
	if (SpawnParams.Num() > 0)
	{
		return SpawnParams.Pop();
	}
	UE_LOG(LogAura, Error, TEXT("[UAuraSpawnBlueprintLibrary.Pop] Attempted to Pop off an empty array!"))
	return FAuraSpawnParams();
}

bool UAuraSpawnBlueprintLibrary::IsValid(const FAuraSpawnParams& SpawnParams)
{
	return SpawnParams.IsValid();
}

FVector UAuraSpawnBlueprintLibrary::GetGroundLocation(const AActor* Actor)
{
	if (!Actor)
	{
		return FVector::ZeroVector;
	}
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Actor);
	if (Actor->GetWorld()->LineTraceSingleByChannel(Hit, Actor->GetActorLocation(), Actor->GetActorLocation() + (FVector::DownVector * 1000), ECC_WorldStatic, Params))
	{
		return Hit.ImpactPoint;
	}
	return Actor->GetActorLocation();
}
