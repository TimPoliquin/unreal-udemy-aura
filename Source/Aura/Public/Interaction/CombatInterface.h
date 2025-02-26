// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

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
	TArray<FName> GetTargetTagsToIgnore();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector GetCombatSocketLocation() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAnimMontage* GetHitReactMontage();

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
};
