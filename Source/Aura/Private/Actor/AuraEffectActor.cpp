// Copyright Alien Shores


#include "Actor/AuraEffectActor.h"

#include "Item/Component/AuraEffectComponent.h"
#include "Kismet/GameplayStatics.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = true;
	EffectComponent = CreateDefaultSubobject<UAuraEffectComponent>(TEXT("Effect Component"));
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
	if (bPlaySpawnEffect)
	{
		PlaySpawnEffect();
		if (SpawnSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, SpawnSound, GetActorLocation(), GetActorRotation());
		}
	}
}
