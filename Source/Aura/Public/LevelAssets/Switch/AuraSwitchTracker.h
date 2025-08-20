// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/Component/AuraLockComponent.h"
#include "AuraSwitchTracker.generated.h"

class UAuraLockComponent;

UCLASS()
class AURA_API AAuraSwitchTracker : public AActor
{
	GENERATED_BODY()

public:
	AAuraSwitchTracker();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	void OnUnlock(
		UPARAM(ref)
		const FOnAuraLockComponentUnlockPayload& Payload
	);
	UFUNCTION(BlueprintNativeEvent)
	void OnReset();
	UFUNCTION(BlueprintNativeEvent)
	void OnUnlockProgress(const FOnAuraLockComponentUnlockProgressPayload& Payload);

	UFUNCTION(BlueprintCallable)
	void PlaySuccessSound(EAuraLockSFXPlaybackLocation PlaybackLocation = EAuraLockSFXPlaybackLocation::Default);
	UFUNCTION(BlueprintCallable)
	void PlayResetSound(const EAuraLockSFXPlaybackLocation PlaybackLocation = EAuraLockSFXPlaybackLocation::Default);
	UFUNCTION(BlueprintCallable)
	void PlayCompletedSound(const EAuraLockSFXPlaybackLocation PlaybackLocation = EAuraLockSFXPlaybackLocation::Default);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UAuraLockComponent> LockComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Properties", meta=(ObjectMustImplement="Script/Aura.AuraLockedInterface"))
	TArray<AActor*> LockedActors;
};
