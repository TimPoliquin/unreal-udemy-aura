// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "AuraLockComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAuraLockComponentUnlockSignature);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AURA_API UAuraLockComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAuraLockComponent();
	FOnAuraLockComponentUnlockSignature OnUnlockDelegate;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsPreconditionMet(const AActor* Player) const;
	UFUNCTION(BlueprintCallable)
	bool TryUnlock(AActor* Player);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsUnlocked() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsKeyRequiredToUnlock() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FString GetUnlockText() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Lock", meta=(Categories="Item.Type.Key"))
	FGameplayTag KeyTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Lock")
	bool bConsumesKey = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Lock")
	TObjectPtr<USoundBase> UnlockSound;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category="Lock")
	bool bUnlocked = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Lock")
	FString UnlockText = FString("Unlock");
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Lock")
	FString OpenText = FString("Open");
};
