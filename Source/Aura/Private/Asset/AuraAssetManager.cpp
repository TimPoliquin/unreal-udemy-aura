// Copyright Alien Shores


#include "Asset/AuraAssetManager.h"

#include "Tags/AuraGameplayTags.h"
#include "AbilitySystemGlobals.h"

UAuraAssetManager& UAuraAssetManager::Get()
{
	return *Cast<UAuraAssetManager>(GEngine->AssetManager);
}

void UAuraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	FAuraGameplayTags::InitializeNativeGameplayTags();
	// DEVNOTE: Required to use TargetData (TargetDataUnderMouse)
	UAbilitySystemGlobals::Get().InitGlobalData();
}
