// Copyright Alien Shores


#include "Actor/Spawn/AuraSpawnBlueprintLibrary.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
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
	return SpawnParams.Pop();
}
