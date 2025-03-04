// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * AuraGameplayTags is a singleton containing native Gameplay tags
 */
struct FAuraGameplayTags
{
	static FAuraGameplayTags& Get()
	{
		if (!Instance.bInitialized)
		{
			Instance.InitializeNativeGameplayTags();
			Instance.bInitialized = true;
		}
		return Instance;
	}

	static void InitializeNativeGameplayTags();
	static bool IsLeftMouseButton(const FGameplayTag&);

	bool bInitialized = false;

	/** Primary attributes **/
	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Resilience;
	FGameplayTag Attributes_Primary_Vigor;
	/** Secondary Attributes **/
	FGameplayTag Attributes_Secondary_Armor;
	FGameplayTag Attributes_Secondary_ArmorPenetration;
	FGameplayTag Attributes_Secondary_BlockChance;
	FGameplayTag Attributes_Secondary_CriticalHitChance;
	FGameplayTag Attributes_Secondary_CriticalHitDamage;
	FGameplayTag Attributes_Secondary_CriticalHitResistance;
	FGameplayTag Attributes_Secondary_HealthRegeneration;
	FGameplayTag Attributes_Secondary_ManaRegeneration;
	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_MaxMana;
	/** Vital Attributes **/
	FGameplayTag Attributes_Vital_Health;
	FGameplayTag Attributes_Vital_Mana;

	/** Input Tags **/
	FGameplayTag InputTag_LeftMouseButton;
	FGameplayTag InputTag_AttackTarget;
	FGameplayTag InputTag_RightMouseButton;
	FGameplayTag InputTag_Action1;
	FGameplayTag InputTag_Action2;
	FGameplayTag InputTag_Action3;
	FGameplayTag InputTag_Action4;

	/** Effect Tags **/
	FGameplayTag Effect_HitReact;

	/** Damage Tags **/
	FGameplayTag Damage;
	FGameplayTag Damage_Arcane;
	FGameplayTag Damage_Fire;
	FGameplayTag Damage_Lightning;
	FGameplayTag Damage_Physical;

	/** Resistance Tags **/
	FGameplayTag Attributes_Resistance_Arcane;
	FGameplayTag Attributes_Resistance_Fire;
	FGameplayTag Attributes_Resistance_Lightning;
	FGameplayTag Attributes_Resistance_Physical;

	/** Abilities **/
	FGameplayTag Abilities_Attack;

	/** Combat Socket Tags */
	FGameplayTag Combat_Socket_Weapon;
	FGameplayTag Combat_Socket_LeftHand;
	FGameplayTag Combat_Socket_RightHand;

	/**
	 * Montage Attack Tags 
	 */
	uint8 MontageAttackNum = 4;
	TArray<FGameplayTag> Montage_Attack_Tags;

	FORCEINLINE const TArray<FGameplayTag>& GetDamageTypes() const
	{
		return DamageTypes;
	}

	FORCEINLINE FGameplayTag GetDamageTypeResistanceTag(const FGameplayTag& DamageType) const
	{
		return DamageTypesToResistances[DamageType];
	}

private:
	static FAuraGameplayTags Instance;
	TArray<FGameplayTag> DamageTypes;
	TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances;
};
