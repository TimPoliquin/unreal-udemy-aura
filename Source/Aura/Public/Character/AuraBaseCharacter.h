// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interaction/CombatInterface.h"
#include "AuraBaseCharacter.generated.h"

class UNiagaraSystem;
struct FGameplayTag;
class UGameplayAbility;
class UGameplayEffect;
class UAttributeSet;
class UAbilitySystemComponent;

UCLASS(Abstract)
class AURA_API AAuraBaseCharacter : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	AAuraBaseCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const;

	/** Combat Interface **/
	virtual AActor* GetAvatar_Implementation() override;
	virtual UAnimMontage* GetHitReactMontage_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() const override;
	virtual FTaggedMontage GetTagMontageByTag_Implementation(const FGameplayTag& MontageTag) override;
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) const override;
	virtual void Die() override;
	virtual bool IsDead_Implementation() const override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;
	virtual int32 GetXPReward_Implementation() const override;

	virtual TArray<FName> GetTargetTagsToIgnore_Implementation() const override
	{
		return TArray<FName>();
	};
	virtual int32 GetMinionCount_Implementation() const override;
	virtual void ChangeMinionCount_Implementation(const int32 Delta) override;
	/** Combat Interface End **/

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath();

protected:
	virtual void BeginPlay() override;

	virtual void InitializeAbilityActorInfo()
	{
	};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> InitializeVitalAttributes;

	virtual void InitializeDefaultAttributes() const;

	void AddCharacterAbilities();
	virtual void OnHitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bHitReacting;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float BaseWalkSpeed = 250.f;

	/** Dissolve Effect */
	void Dissolve();
	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(UMaterialInstanceDynamic* DisolveMaterialInstance);
	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* DisolveMaterialInstance);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Materials|Disolve")
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Materials|Disolve")
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UNiagaraSystem> BloodEffect;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	TObjectPtr<USoundBase> DeathSound;

	/** Minions **/
	int32 MinionCount = 0;

private:
	bool bDead = false;
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartingAbilities;
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartingPassiveAbilities;
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FTaggedMontage> AttackMontages;

	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> Attributes, const float Level) const;
	void Dissolve(
		UMeshComponent* Mesh,
		UMaterialInstance* MaterialInstance,
		void (AAuraBaseCharacter::*Callback)(UMaterialInstanceDynamic*)
	);
};
