// Copyright Alien Shores


#include "AbilitySystem/Calculations/ExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAbilitySystemTypes.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Tags/AuraGameplayTags.h"

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resistance_Arcane);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resistance_Fire);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resistance_Lightning);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resistance_Physical);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Passive_Protection);

	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;

	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Resistance_Arcane, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Resistance_Fire, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Resistance_Lightning, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Resistance_Physical, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Passive_Protection, Target, false);

		const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();

		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Armor, ArmorDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_BlockChance, BlockChanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_ArmorPenetration, ArmorPenetrationDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitChance, CriticalHitChanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitResistance, CriticalHitResistanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitDamage, CriticalHitDamageDef);

		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Arcane, Resistance_ArcaneDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Fire, Resistance_FireDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Lightning, Resistance_LightningDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Physical, Resistance_PhysicalDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Passive_Protection, Passive_ProtectionDef);
	}
};

static const AuraDamageStatics& DamageStatics()
{
	// static singleton 
	static AuraDamageStatics DStatics;
	return DStatics;
};

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().Resistance_ArcaneDef);
	RelevantAttributesToCapture.Add(DamageStatics().Resistance_FireDef);
	RelevantAttributesToCapture.Add(DamageStatics().Resistance_LightningDef);
	RelevantAttributesToCapture.Add(DamageStatics().Resistance_PhysicalDef);
	RelevantAttributesToCapture.Add(DamageStatics().Passive_ProtectionDef);
}

float UExecCalc_Damage::CalculateBaseDamage(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	const FAggregatorEvaluateParameters& EvaluateParameters
)
{
	FGameplayEffectContextHandle EffectContextHandle;
	float Damage = 0.f;
	for (const auto& DamageType : FAuraGameplayTags::Get().GetDamageTypes())
	{
		const float DamageTypeDamage = GetDamageTypeDamage(ExecutionParams, EvaluateParameters, DamageType);
		if (DamageTypeDamage > 0.f)
		{
			Damage += DamageTypeDamage;
			UAuraAbilitySystemLibrary::SetDamageTypeTag(EffectContextHandle, DamageType);
			break;
		}
	}
	return Damage;
}

void UExecCalc_Damage::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput
) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// Debuff
	DetermineDebuff(ExecutionParams, EvaluateParameters);

	// Get Damage Set by Caller Magnitude
	float Damage = CalculateBaseDamage(ExecutionParams, EvaluateParameters);
	if (IsRadialDamage(ExecutionParams))
	{
		ApplyRadialDamage(ExecutionParams, Damage);
	}
	// If the attack was blocked (based on BlockChance), cut the damage in half.
	if (IsAttackBlockedByTarget(ExecutionParams, EvaluateParameters))
	{
		Damage *= .5f;
		UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, true);
	}
	// Reduce damage by a percentage based on target's effective armor and level of protection
	Damage *= (100 - GetTargetEffectiveArmor(ExecutionParams, EvaluateParameters)) / 100.f;
	// if the attack is a critical hit, increase the damage by the critical hit damage
	if (IsCriticalHitOnTarget(ExecutionParams, EvaluateParameters))
	{
		Damage = Damage * 2.f + GetEffectiveCriticalHitDamage(
			ExecutionParams,
			EvaluateParameters
		);
		UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, true);
	}

	const FGameplayModifierEvaluatedData EvaluatedData(
		UAuraAttributeSet::GetMeta_IncomingDamageAttribute(),
		EGameplayModOp::Additive,
		Damage
	);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}

float UExecCalc_Damage::GetDamageTypeDamage(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	const FAggregatorEvaluateParameters& EvaluateParameters,
	const FGameplayTag& DamageTypeTag
)
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTag ResistanceTag = FAuraGameplayTags::Get().GetDamageTypeResistanceTag(DamageTypeTag);
	const FGameplayEffectAttributeCaptureDefinition* ResistanceDef = DamageStatics().TagsToCaptureDefs.Find(
		ResistanceTag
	);
	checkf(
		ResistanceDef,
		TEXT("TagsToCaptureDefs doesn't contain Tag: [%s]"),
		*ResistanceTag.ToString()
	);
	const float Damage = Spec.GetSetByCallerMagnitude(DamageTypeTag, false);
	float Resistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		*ResistanceDef,
		EvaluateParameters,
		Resistance
	);
	Resistance = FMath::Clamp(Resistance, 0.f, 100.f);
	return Damage * (100.f - Resistance) / 100.f;
}

bool UExecCalc_Damage::IsAttackBlockedByTarget(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	const FAggregatorEvaluateParameters& EvaluateParameters
)
{
	float BlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().BlockChanceDef,
		EvaluateParameters,
		BlockChance
	);
	const float BlockCalc = FMath::RandRange(1, 100);
	return BlockCalc <= BlockChance;
}

float UExecCalc_Damage::GetTargetEffectiveArmor(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	const FAggregatorEvaluateParameters& EvaluateParameters
)
{
	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(
		ExecutionParams.GetSourceAbilitySystemComponent()->GetAvatarActor()
	);
	// target protection
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().ArmorDef,
		EvaluateParameters,
		TargetArmor
	);
	TargetArmor = FMath::Max<float>(TargetArmor, 0.f);
	float PassiveProtectionPercentage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().Passive_ProtectionDef,
		EvaluateParameters,
		PassiveProtectionPercentage
	);
	PassiveProtectionPercentage = FMath::Max(PassiveProtectionPercentage, 1.f, PassiveProtectionPercentage);
	TargetArmor *= PassiveProtectionPercentage;
	const float EffectiveArmorCoefficient =
		CharacterClassInfo->GetEffectiveArmorCoefficient(
			ICombatInterface::GetCharacterLevel(ExecutionParams.GetTargetAbilitySystemComponent())
		);
	// source armor penetration
	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().ArmorPenetrationDef,
		EvaluateParameters,
		SourceArmorPenetration
	);

	const float ArmorPenetrationCoefficient =
		CharacterClassInfo->GetArmorPenetrationCoefficient(
			ICombatInterface::GetCharacterLevel(ExecutionParams.GetSourceAbilitySystemComponent())
		);

	const float EffectiveArmor = EffectiveArmorCoefficient * FMath::Max<float>(
		TargetArmor * (100 - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f,
		0.f
	);
	return EffectiveArmor;
}

bool UExecCalc_Damage::IsCriticalHitOnTarget(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	const FAggregatorEvaluateParameters& EvaluateParameters
)
{
	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().CriticalHitChanceDef,
		EvaluateParameters,
		SourceCriticalHitChance
	);
	SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance, 0.f);
	float TargetCriticalHitResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().CriticalHitResistanceDef,
		EvaluateParameters,
		TargetCriticalHitResistance
	);
	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(
		ExecutionParams.GetSourceAbilitySystemComponent()
	);
	TargetCriticalHitResistance *= CharacterClassInfo->GetCriticalHitResistanceCoefficient(
		ICombatInterface::GetCharacterLevel(ExecutionParams.GetTargetAbilitySystemComponent())
	);
	const float CriticalHitCalculation = FMath::RandRange(1, 100) - TargetCriticalHitResistance;
	return CriticalHitCalculation <= SourceCriticalHitChance;
}

float UExecCalc_Damage::GetEffectiveCriticalHitDamage(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	const FAggregatorEvaluateParameters& EvaluateParameters
)
{
	float SourceCriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().CriticalHitDamageDef,
		EvaluateParameters,
		SourceCriticalHitDamage
	);
	return SourceCriticalHitDamage;
}

bool UExecCalc_Damage::IsDebuffApplied(
	const FGameplayTag& DamageTypeTag,
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	const FAggregatorEvaluateParameters& EvaluateParameters
)
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	const FGameplayEffectSpec Spec = ExecutionParams.GetOwningSpec();
	if (Spec.GetSetByCallerMagnitude(DamageTypeTag, false, -1.f) > 0.f)
	{
		const FGameplayTag& ResistanceTag = GameplayTags.GetDamageTypeResistanceTag(DamageTypeTag);
		const float SourceDebuffChance = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Stat_Chance, false, -1.f);
		float TargetDebuffResistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
			DamageStatics().TagsToCaptureDefs[ResistanceTag],
			EvaluateParameters,
			TargetDebuffResistance
		);
		TargetDebuffResistance = FMath::Max<float>(TargetDebuffResistance, 0.f);
		// DEVNOTE - this was a shorthand hack by the course instructor to make each point of resistance reduce debuff chance by 1%.
		// In a real project, this would probably be done via game config (curve table, etc)
		const float EffectiveDebuffChance = SourceDebuffChance * (100 - TargetDebuffResistance) / 100.f;
		return FMath::RandRange(1, 100) < EffectiveDebuffChance;
	}
	return false;
}

void UExecCalc_Damage::DetermineDebuff(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	const FAggregatorEvaluateParameters& EvaluateParams
)
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	const FGameplayEffectSpec Spec = ExecutionParams.GetOwningSpec();

	for (const FGameplayTag& DamageTypeTag : GameplayTags.GetDamageTypes())
	{
		if (IsDebuffApplied(DamageTypeTag, ExecutionParams, EvaluateParams))
		{
			FGameplayEffectContextHandle EffectContext = Spec.GetContext();
			FGameplayTag DebuffTypeTag = GameplayTags.GetDamageTypeDebuffTag(DamageTypeTag);
			UAuraAbilitySystemLibrary::SetDebuff(
				EffectContext,
				DebuffTypeTag,
				Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Stat_Damage, false, -1.f),
				Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Stat_Duration, false, -1.f),
				Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Stat_Frequency, false, -1.f)
			);
			break;
		}
	}
}

bool UExecCalc_Damage::IsRadialDamage(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams
)
{
	return UAuraAbilitySystemLibrary::IsRadialDamage(ExecutionParams.GetOwningSpec().GetContext());
}

void UExecCalc_Damage::ApplyRadialDamage(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	float& OutRadialDamage
)
{
	const FGameplayEffectSpec Spec = ExecutionParams.GetOwningSpec();
	const UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	AActor* TargetActor = TargetAbilitySystemComponent
		                      ? TargetAbilitySystemComponent->GetAvatarActor()
		                      : nullptr;
	const UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();
	AActor* SourceActor = SourceAbilitySystemComponent
		                      ? SourceAbilitySystemComponent->GetAvatarActor()
		                      : nullptr;
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(TargetActor))
	{
		FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
		CombatInterface->GetOnDamageDelegate().AddLambda(
			[&](float DamageAmount)
			{
				OutRadialDamage = DamageAmount;
			}
		);
		UGameplayStatics::ApplyRadialDamageWithFalloff(
			TargetActor,
			OutRadialDamage,
			0.f,
			UAuraAbilitySystemLibrary::GetRadialDamageOrigin(EffectContextHandle),
			UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(EffectContextHandle),
			UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(EffectContextHandle),
			1.f,
			UDamageType::StaticClass(),
			TArray<AActor*>(),
			SourceActor,
			nullptr
		);
	}
}
