// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class ULevelUpInfo;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangeSignature, const int32, Value);


class UAttributeSet;
class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAuraPlayerState();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	UAttributeSet* GetAttributeSet() const;
	FORCEINLINE int32 GetCharacterLevel() const
	{
		return Level;
	}

	FORCEINLINE void SetLevel(const int32 NewLevel)
	{
		Level = NewLevel;
		OnLevelChangeDelegate.Broadcast(Level);
	}

	FORCEINLINE void AddToLevel(const int32 AddLevel)
	{
		SetLevel(Level + AddLevel);
	}

	FORCEINLINE int32 GetXP() const
	{
		return XP;
	}

	FORCEINLINE void SetXP(const int32 InXP)
	{
		XP = InXP;
		OnXPChangeDelegate.Broadcast(XP);
	}

	FORCEINLINE void AddToXP(const int32 InXP)
	{
		SetXP(InXP + XP);
	}

	float GetXPToNextLevelPercentage() const;

	FOnPlayerStatChangeSignature OnXPChangeDelegate;
	FOnPlayerStatChangeSignature OnLevelChangeDelegate;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo;
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Level)
	int32 Level = 1;
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_XP)
	int32 XP = 0;

	UFUNCTION()
	FORCEINLINE void OnRep_Level(int32 OldLevel) const
	{
		OnLevelChangeDelegate.Broadcast(OldLevel);
	}

	UFUNCTION()
	FORCEINLINE void OnRep_XP(int32 OldXP) const
	{
		OnXPChangeDelegate.Broadcast(OldXP);
	}
};
