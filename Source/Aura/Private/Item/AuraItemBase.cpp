// Copyright Alien Shores


#include "Item/AuraItemBase.h"


// Sets default values
AAuraItemBase::AAuraItemBase()
{
	PrimaryActorTick.bCanEverTick = false;
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(GetRootComponent());
}

USkeletalMeshComponent* AAuraItemBase::GetMesh() const
{
	return MeshComponent;
}
