// Copyright Alien Shores


#include "Fishing/AuraWaterRipple.h"


AAuraWaterRipple::AAuraWaterRipple()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAuraWaterRipple::BeginPlay()
{
	Super::BeginPlay();
	if (bStartRippling)
	{
		PlayRipple(true);
	}
	else
	{
		GetRootComponent()->SetVisibility(false, true);
	}
}
