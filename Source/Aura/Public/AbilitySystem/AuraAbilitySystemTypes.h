#pragma once
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "ScalableFloat.h"
#include "AuraAbilitySystemTypes.generated.h"

USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}

	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;

	bool IsCriticalHit() const
	{
		return bIsCriticalHit;
	}

	void SetIsCriticalHit(const bool bIn)
	{
		bIsCriticalHit = bIn;
	}

	bool IsBlockedHit() const
	{
		return bIsBlockedHit;
	}

	void SetIsBlockedHit(const bool bIn)
	{
		this->bIsBlockedHit = bIn;
	}

	/** Creates a copy of this context, used to duplicate for later modifications */
	virtual FAuraGameplayEffectContext* Duplicate() const override
	{
		FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

protected:
	UPROPERTY()
	bool bIsBlockedHit = false;
	UPROPERTY()
	bool bIsCriticalHit = false;

private:
	void GetSavingBits(uint32& RepBits) const;
	void AssignValuesToArchive(FArchive& Ar, UPackageMap* Map, const uint32& RepBits, bool& bOutSuccess);
};

template <>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true,
	};
};

USTRUCT()
struct FAbilityTagStatus
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag AbilityTag;
	UPROPERTY()
	FGameplayTag StatusTag;
	UPROPERTY()
	int32 AbilityLevel;

	static FAbilityTagStatus Create(
		const FGameplayTag AbilityTag,
		const FGameplayTag StatusTag,
		const int32 AbilityLevel
	)
	{
		FAbilityTagStatus AbilityStatus;
		AbilityStatus.AbilityTag = AbilityTag;
		AbilityStatus.StatusTag = StatusTag;
		AbilityStatus.AbilityLevel = AbilityLevel;
		return AbilityStatus;
	}

	static TArray<FAbilityTagStatus> CreateArray(
		const FGameplayTag AbilityTag,
		const FGameplayTag StatusTag,
		const int32 AbilityLevel
	)
	{
		TArray<FAbilityTagStatus> AbilityStatus;
		AbilityStatus.Add(Create(AbilityTag, StatusTag, AbilityLevel));
		return AbilityStatus;
	}
};

USTRUCT(BlueprintType)
struct FAuraAbilityDescription
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	FString Description = FString();
	UPROPERTY(BlueprintReadOnly)
	FString NextLevelDescription = FString();
};

USTRUCT(BlueprintType)
struct FAuraEquipAbilityPayload
{
	GENERATED_BODY()
	UPROPERTY()
	FGameplayTag AbilityTag;
	UPROPERTY()
	FGameplayTag StatusTag;
	UPROPERTY()
	FGameplayTag SlotTag;
	UPROPERTY()
	FGameplayTag PreviousSlotTag;

	static FAuraEquipAbilityPayload Create(
		const FGameplayTag& AbilityTag,
		const FGameplayTag& StatusTag,
		const FGameplayTag& SlotTag,
		const FGameplayTag& PreviousSlotTag
	)
	{
		FAuraEquipAbilityPayload EquipPayload;
		EquipPayload.AbilityTag = AbilityTag;
		EquipPayload.StatusTag = StatusTag;
		EquipPayload.SlotTag = SlotTag;
		EquipPayload.PreviousSlotTag = PreviousSlotTag;
		return EquipPayload;
	}
};

USTRUCT(BlueprintType)
struct FAuraDamageConfig
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag DamageTypeTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FScalableFloat Amount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DebuffChance = 20.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DebuffDamage = 5.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DebuffFrequency = 1.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DebuffDuration = 5.f;
};

USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()
	FDamageEffectParams()
	{
	}

	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject = nullptr;
	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent = nullptr;
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent = nullptr;
	UPROPERTY()
	float BaseDamage = 0.f;
	UPROPERTY()
	float AbilityLevel = 1.f;
	UPROPERTY()
	FGameplayTag DamageType = FGameplayTag::EmptyTag;
	UPROPERTY()
	float DebuffChance = 0.f;
	UPROPERTY()
	float DebuffDamage = 0.f;
	UPROPERTY()
	float DebuffDuration = 0.f;
	UPROPERTY()
	float DebuffFrequency = 0.f;

	void FillFromDamageConfig(const FAuraDamageConfig& DamageConfig)
	{
		DamageType = DamageConfig.DamageTypeTag;
		DebuffChance = DamageConfig.DebuffChance;
		DebuffDamage = DamageConfig.DebuffDamage;
		DebuffDuration = DamageConfig.DebuffDuration;
		DebuffFrequency = DamageConfig.DebuffFrequency;
	}
};
