// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/AttributeChangeDelegates.h"
#include "Interaction/CombatInterface.h"
#include "AuraBaseCharacter.generated.h"

class UPassiveNiagaraComponent;
class UDebuffNiagaraComponent;
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
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const;
	virtual float TakeDamage(
		float DamageAmount,
		const struct FDamageEvent& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
	) override;

	/** Combat Interface **/
	virtual AActor* GetAvatar_Implementation() override;
	virtual UAnimMontage* GetHitReactMontage_Implementation(const FGameplayTag& HitReactTypeTag) override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() const override;
	virtual FTaggedMontage GetTagMontageByTag_Implementation(const FGameplayTag& MontageTag) override;
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) const override;
	virtual void Die() override;
	virtual bool IsDead_Implementation() const override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;
	virtual int32 GetXPReward_Implementation() const override;

	virtual FOnDeathSignature& GetOnDeathDelegate() override
	{
		return OnDeathDelegate;
	}

	virtual FOnAbilitySystemComponentRegisteredSignature& GetOnAbilitySystemRegisteredDelegate() override
	{
		return OnAbilitySystemComponentRegisteredDelegate;
	}

	virtual FOnDamageSignature& GetOnDamageDelegate() override
	{
		return OnDamageDelegate;
	}

	virtual TArray<FName> GetTargetTagsToIgnore_Implementation() const override
	{
		return TArray<FName>();
	};
	virtual int32 GetMinionCount_Implementation() const override;
	virtual void ChangeMinionCount_Implementation(const int32 Delta) override;
	virtual void ApplyDeathImpulse(const FVector& DeathImpulse) override;

	virtual USkeletalMeshComponent* GetWeapon_Implementation() const override
	{
		return nullptr;
	}

	virtual void SetActiveAbilityTag_Implementation(const FGameplayTag& InActiveAbilityTag) override
	{
		ActiveAbilityTag = InActiveAbilityTag;
	}

	virtual void ClearActiveAbilityTag_Implementation() override
	{
		ActiveAbilityTag = FGameplayTag::EmptyTag;
	}

	virtual FGameplayTag
	GetHitReactAbilityTagByDamageType_Implementation(const FGameplayTag& DamageTypeTag) const override;

	/** Combat Interface End **/

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath();

protected:
	virtual void BeginPlay() override;

	virtual void InitializeAbilityActorInfo()
	{
	};

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

	virtual void InitializeDefaultAttributes();

	void AddCharacterAbilities();
	UFUNCTION()
	virtual void OnHitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	UFUNCTION(BlueprintCallable)
	bool IsShocked() const;
	UFUNCTION(BlueprintCallable)
	bool IsBurned() const;

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat")
	TMap<FGameplayTag, FGameplayTag> HitReactionsByDamageType;
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bHitReacting;
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	TMap<FGameplayTag, TObjectPtr<UAnimMontage>> HitReactionMontageByMontageTag;
	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FTaggedMontage> AttackMontages;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UNiagaraSystem> BloodEffect;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	TObjectPtr<USoundBase> DeathSound;

	UPROPERTY(VisibleAnywhere, Category="Combat|Debuff")
	TObjectPtr<UDebuffNiagaraComponent> BurnDebuffComponent;
	UPROPERTY(VisibleAnywhere, Category="Combat|Debuff")
	TObjectPtr<UDebuffNiagaraComponent> ShockDebuffComponent;
	UPROPERTY(VisibleAnywhere, Category="Combat|Passive")
	TObjectPtr<USceneComponent> EffectAttachComponent;
	UPROPERTY(VisibleAnywhere, Category = "Combat|Passive")
	TObjectPtr<UPassiveNiagaraComponent> HaloOfProtectionNiagaraComponent;
	UPROPERTY(VisibleAnywhere, Category = "Combat|Passive")
	TObjectPtr<UPassiveNiagaraComponent> LifeSiphonNiagaraComponent;
	UPROPERTY(VisibleAnywhere, Category = "Combat|Passive")
	TObjectPtr<UPassiveNiagaraComponent> ManaSiphonNiagaraComponent;


	/** Minions **/
	int32 MinionCount = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_ActiveAbilityTag, Category = "Combat")
	FGameplayTag ActiveAbilityTag;
	UFUNCTION()
	virtual void OnRep_ActiveAbilityTag()
	{
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_StatusEffectTags, Category = "Combat")
	TArray<FGameplayTag> StatusEffectTags;

	UFUNCTION()
	virtual void OnRep_StatusEffectTags()
	{
	}

	virtual void OnStatusShockAdded()
	{
	};

	virtual void OnStatusShockRemoved()
	{
	};

	virtual void OnStatusBurnAdded()
	{
	};

	virtual void OnStatusBurnRemoved()
	{
	};

private:
	bool bDead = false;
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartingAbilities;
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartingPassiveAbilities;
	FOnAbilitySystemComponentRegisteredSignature OnAbilitySystemComponentRegisteredDelegate;
	FOnDeathSignature OnDeathDelegate;
	FOnDamageSignature OnDamageDelegate;
	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> Attributes, const float Level) const;
	void Dissolve(
		UMeshComponent* InMesh,
		UMaterialInstance* MaterialInstance,
		void (AAuraBaseCharacter::*Callback)(UMaterialInstanceDynamic*)
	);
	UFUNCTION()
	void OnDebuffTypeBurnChanged(FGameplayTag GameplayTag, int StackCount);
	UFUNCTION()
	void OnDebuffTypeShockChanged(FGameplayTag StunTag, int32 Count);
	UFUNCTION()
	void RegisterStatusEffectTags(UAbilitySystemComponent* InAbilitySystemComponent);
};
