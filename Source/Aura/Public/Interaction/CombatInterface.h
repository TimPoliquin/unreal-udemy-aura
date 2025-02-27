// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FTaggedMontage
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* Montage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag MontageTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName SocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ImpactRadius = 45.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* ImpactSound = nullptr;
};

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual int32 GetCharacterLevel() const;
	virtual void Die() = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsDead() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetAvatar();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<FName> GetTargetTagsToIgnore() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<FTaggedMontage> GetAttackMontages() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector GetCombatSocketLocation(const FGameplayTag& MontageTag) const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAnimMontage* GetHitReactMontage();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UNiagaraSystem* GetBloodEffect();

	/**
	 * 
	 * @param Actor 
	 * @return Get the level of the actor if it implements ICombatInterface. -1 otherwise.
	 */
	static int32 GetCharacterLevel(const AActor* Actor);
	/**
	 * 
	 * @param Actor 
	 * @return true if the provided actor implements ICombatInterface and is alive (not dead).
	 */
	static bool IsAlive(const AActor* Actor);
	/**
	 * 
	 * @param Actor
	 * @return true if the provided actor implements ICombatInterface and is dead.
	 */
	static bool IsDead(const AActor* Actor);

	static TArray<FTaggedMontage> GetAttackMontages(const AActor* Actor);

	static TArray<FName> GetTargetTagsToIgnore(const AActor* Actor);
};
