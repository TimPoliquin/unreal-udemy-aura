// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraBaseCharacter.h"
#include "EnemyInterface.h"
#include "Interaction/HighlightInterface.h"
#include "AbilitySystem/AttributeChangeDelegates.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Actor/Spawn/TrackableInterface.h"
#include "AuraEnemy.generated.h"

class UBehaviorTree;
class AAuraAIController;
class UWidgetComponent;

UCLASS()
class AURA_API AAuraEnemy : public AAuraBaseCharacter, public IHighlightInterface, public IEnemyInterface, public ITrackableInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* NewController) override;

	virtual void OnHitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount) override;

	// IHighlightInterface
	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;

	virtual bool SetMoveToLocation_Implementation(FVector& OutDestination) override
	{
		// do not override destination
		return false;
	};
	// ICombatInterface
	FORCEINLINE virtual int GetCharacterLevel_Implementation() const override
	{
		return Level;
	}

	virtual int32 GetXPReward_Implementation() const override;
	virtual USkeletalMeshComponent* GetWeapon_Implementation() const override;

	virtual TArray<FName> GetTargetTagsToIgnore_Implementation() const override;

	virtual void Die() override;
	// IEnemyInterface
	FORCEINLINE virtual AActor* GetCombatTarget_Implementation() const override
	{
		return CombatTarget;
	}

	FORCEINLINE virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override
	{
		CombatTarget = InCombatTarget;
	}

	/** Start ITrackableInterface **/
	virtual FOnTrackableStopTrackingSignature& GetStopTrackingDelegate() override;
	/** End ITrackableInterface **/

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;

	void SetLevel(const int32 InLevel) { Level = InLevel; }
	ECharacterClass GetCharacterClass() const;
	void SetCharacterClass(const ECharacterClass InCharacterClass);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SpawnLoot();

protected:
	virtual void BeginPlay() override;
	virtual void InitializeAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() override;
	virtual void OnStatusShockAdded() override;
	virtual void OnStatusShockRemoved() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Highlight")
	bool bHighlighted;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> HealthWidget;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float LifeSpan = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;
	UPROPERTY()
	TObjectPtr<AAuraAIController> AuraAIController;
	/** Attack range for the enemy. Note: This should probably be in the ability instead of the enemy. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float AttackRange = 500.f;
	/**
	 * Attack Range Tolerance
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float AttackRangeTolerance = 20.f;
	/** Targeting range for enemy. If the enemy detects a player within this range, it will move to attack. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float TargetingRange = 4000.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float AttackWaitTime = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float AttackWaitDeviation = .5f;

	UPROPERTY(BlueprintReadWrite, Category = "AI")
	TObjectPtr<AActor> CombatTarget;

private:
	FOnTrackableStopTrackingSignature OnTrackableStopTracking;

	UPROPERTY(EditAnywhere, Category = "Highlight")
	uint8 HighlightCustomDepthStencilValue = 250;

	void InitializeAttributeDelegates();
	void InitializeStartupAbilities() const;
};
