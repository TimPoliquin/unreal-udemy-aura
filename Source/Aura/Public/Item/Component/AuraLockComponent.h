// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "LevelAssets/Switch/SwitchDelegates.h"
#include "AuraLockComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAuraLockComponentUnlockSignature);

UENUM(BlueprintType)
enum class EAuraUnlockMode : uint8
{
	Unlocked,
	Key,
	Switch,
	Custom,
};


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
	bool IsLocked() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsManuallyUnlockable() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FString GetUnlockText() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Lock")
	EAuraUnlockMode UnlockMode = EAuraUnlockMode::Unlocked;
	/** Key Properties **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Lock|Key", meta=(Categories="Item.Type.Key", EditCondition="UnlockMode == EAuraUnlockMode::Key", EditConditionHides))
	FGameplayTag KeyTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Lock|Key", meta=(EditCondition="UnlockMode == EAuraUnlockMode::Key", EditConditionHides))
	bool bConsumesKey = true;
	/** Switch Properties **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Lock|Switch",
		meta=(EditCondition="UnlockMode == EAuraUnlockMode::Switch", EditConditionHides, ObjectMustImplement="/Script/Aura.SwitchInterface"))
	TArray<TObjectPtr<AActor>> Switches;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Lock|Switch", meta=(EditCondition="UnlockMode == EAuraUnlockMode::Switch", EditConditionHides))
	bool bOrdered = false;
	/** Properties **/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category="Properties")
	bool bUnlocked = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	FString UnlockText = FString("Unlock");
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	FString OpenText = FString("Open");

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	bool IsPreconditionMet_Key(const AActor* Player) const;
	UFUNCTION(BlueprintNativeEvent)
	bool IsPreconditionMet_Switch() const;
	UFUNCTION(BlueprintNativeEvent)
	bool IsPreconditionMet_Custom() const;
	UFUNCTION(BlueprintNativeEvent)
	void InitializeUnlock_Key();
	UFUNCTION(BlueprintNativeEvent)
	void InitializeUnlock_Switch();
	UFUNCTION(BlueprintNativeEvent)
	void InitializeUnlock_Custom();
	UFUNCTION(BlueprintNativeEvent)
	void TryUnlock_Key(AActor* Player);
	UFUNCTION(BlueprintNativeEvent)
	void TryUnlock_Switch();
	UFUNCTION(BlueprintNativeEvent)
	void TryUnlock_Custom(AActor* Player);
	UFUNCTION(BlueprintNativeEvent)
	void Unlock(bool bBroadcast = true);

private:
	UFUNCTION()
	void OnSwitchActivated(const FOnSwitchStatusChangedPayload& Payload);
};
