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
	/** Meta Attributes **/
	FGameplayTag Attributes_Meta_IncomingDamage;
	FGameplayTag Attributes_Meta_IncomingXP;

	/** Input Tags **/
	FGameplayTag InputTag;
	FGameplayTag InputTag_LeftMouseButton;
	FGameplayTag InputTag_AttackTarget;
	FGameplayTag InputTag_RightMouseButton;
	FGameplayTag InputTag_Action1;
	FGameplayTag InputTag_Action2;
	FGameplayTag InputTag_Action3;
	FGameplayTag InputTag_Action4;
	FGameplayTag InputTag_Passive1;
	FGameplayTag InputTag_Passive2;

	/** Effect Tags **/
	FGameplayTag Effect_HitReact;
	FGameplayTag Effect_HitReact_Default;
	FGameplayTag Effect_HitReact_Shock;

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

	/** Debuff Tags **/
	FGameplayTag Debuff;
	FGameplayTag Debuff_Type;
	FGameplayTag Debuff_Type_Burn;
	FGameplayTag Debuff_Type_Shock;
	FGameplayTag Debuff_Type_Curse;
	FGameplayTag Debuff_Type_Bleed;
	FGameplayTag Debuff_Stat;
	FGameplayTag Debuff_Stat_Chance;
	FGameplayTag Debuff_Stat_Damage;
	FGameplayTag Debuff_Stat_Frequency;
	FGameplayTag Debuff_Stat_Duration;


	/** Abilities **/
	FGameplayTag Abilities;
	FGameplayTag Abilities_Attack;
	FGameplayTag Abilities_HitReact;
	FGameplayTag Abilities_Summon;

	FGameplayTag Abilities_Offensive;
	FGameplayTag Abilities_Offensive_Fire_FireBolt;
	FGameplayTag Abilities_Offensive_Fire_FireBlast;
	FGameplayTag Abilities_Offensive_Lightning_Electrocute;
	FGameplayTag Abilities_Offensive_Arcane_ArcaneShards;
	FGameplayTag Abilities_Passive;
	FGameplayTag Abilities_Passive_HaloOfProtection;
	FGameplayTag Abilities_Passive_LifeSiphon;
	FGameplayTag Abilities_Passive_ManaSiphon;
	FGameplayTag Abilities_Status;
	FGameplayTag Abilities_Status_Locked;
	FGameplayTag Abilities_Status_Eligible;
	FGameplayTag Abilities_Status_Unlocked;
	FGameplayTag Abilities_Status_Equipped;
	FGameplayTag Abilities_Type;
	FGameplayTag Abilities_Type_Offensive;
	FGameplayTag Abilities_Type_Passive;
	FGameplayTag Abilities_Type_None;

	/** Event Tags **/
	FGameplayTag Event_Montage_Electrocute;

	/** Cooldown **/
	FGameplayTag Cooldown_Fire_FireBolt;
	FGameplayTag Cooldown_Fire_FireBlast;
	FGameplayTag Cooldown_Lightning_Electrocute;
	FGameplayTag Cooldown_Arcane_ArcaneShards;

	/** Combat Socket Tags */
	FGameplayTag Combat_Socket_Weapon;
	FGameplayTag Combat_Socket_LeftHand;
	FGameplayTag Combat_Socket_RightHand;
	FGameplayTag Combat_Socket_Tail;

	/** Player Tags **/
	FGameplayTag Player_Block_CursorTrace;
	FGameplayTag Player_Block_InputPressed;
	FGameplayTag Player_Block_InputHeld;
	FGameplayTag Player_Block_InputReleased;

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

	FORCEINLINE FGameplayTag GetDamageTypeDebuffTag(const FGameplayTag& DamageType) const
	{
		return DamageTypesToDebuffs[DamageType];
	}

	FORCEINLINE FGameplayTag GetDebuffTypeDamageTypeTag(const FGameplayTag& DebuffType) const
	{
		return DebuffTypeToDamageType[DebuffType];
	}

private:
	static FAuraGameplayTags Instance;
	TArray<FGameplayTag> DamageTypes;
	TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances;
	TMap<FGameplayTag, FGameplayTag> DamageTypesToDebuffs;
	TMap<FGameplayTag, FGameplayTag> DebuffTypeToDamageType;
};
