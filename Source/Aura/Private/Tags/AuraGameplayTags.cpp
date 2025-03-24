// Copyright Alien Shores


#include "Tags/AuraGameplayTags.h"
#include "GameplayTagsManager.h"

FAuraGameplayTags FAuraGameplayTags::Instance;

void FAuraGameplayTags::InitializeNativeGameplayTags()
{
	if (Instance.bInitialized)
	{
		return;
	}
	UGameplayTagsManager& TagManager = UGameplayTagsManager::Get();
	Instance.Attributes_Primary_Strength = TagManager.AddNativeGameplayTag(
		FName("Attributes.Primary.Strength"),
		FString("Increases physical damage")
	);
	Instance.Attributes_Primary_Intelligence = TagManager.AddNativeGameplayTag(
		FName("Attributes.Primary.Intelligence"),
		FString("Increases magical damage")
	);
	Instance.Attributes_Primary_Resilience = TagManager.AddNativeGameplayTag(
		FName("Attributes.Primary.Resilience"),
		FString("Increases armor and armor penetration")
	);
	Instance.Attributes_Primary_Vigor = TagManager.AddNativeGameplayTag(
		FName("Attributes.Primary.Vigor"),
		FString("Increases max health and health regen")
	);

	// Secondary Attributes
	Instance.Attributes_Secondary_Armor = TagManager.AddNativeGameplayTag(
		FName("Attributes.Secondary.Armor"),
		FString("Reduces damage taken; improves block chance")
	);
	Instance.Attributes_Secondary_ArmorPenetration = TagManager.AddNativeGameplayTag(
		FName("Attributes.Secondary.ArmorPenetration"),
		FString("")
	);
	Instance.Attributes_Secondary_BlockChance = TagManager.AddNativeGameplayTag(
		FName("Attributes.Secondary.BlockChance"),
		FString("Reduces damage taken, improves Block Chance")
	);
	Instance.Attributes_Secondary_CriticalHitChance = TagManager.AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitChance"),
		FString("")
	);
	Instance.Attributes_Secondary_CriticalHitDamage = TagManager.AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitDamage"),
		FString("")
	);
	Instance.Attributes_Secondary_CriticalHitResistance = TagManager.AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitResistance"),
		FString("")
	);
	Instance.Attributes_Secondary_HealthRegeneration = TagManager.AddNativeGameplayTag(
		FName("Attributes.Secondary.HealthRegeneration"),
		FString("")
	);
	Instance.Attributes_Secondary_ManaRegeneration = TagManager.AddNativeGameplayTag(
		FName("Attributes.Secondary.ManaRegeneration"),
		FString("")
	);
	Instance.Attributes_Secondary_MaxHealth = TagManager.AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxHealth"),
		FString("Maximum health")
	);
	Instance.Attributes_Secondary_MaxMana = TagManager.AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxMana"),
		FString("Maximum mana")
	);
	Instance.Attributes_Vital_Health = TagManager.AddNativeGameplayTag(
		FName("Attributes.Vital.Health"),
		FString("Current Health")
	);
	Instance.Attributes_Vital_Mana = TagManager.AddNativeGameplayTag(
		FName("Attributes.Vital.Mana"),
		FString("Current Mana")
	);
	// Meta Attributes
	Instance.Attributes_Meta_IncomingDamage = TagManager.AddNativeGameplayTag(
		FName("Attributes.Meta.IncomingDamage"),
		FString("Incoming damage")
	);
	Instance.Attributes_Meta_IncomingXP = TagManager.AddNativeGameplayTag(
		FName("Attributes.Meta.IncomingXP"),
		FString("Incoming XP")
	);

	// Input Tags
	Instance.InputTag = TagManager.AddNativeGameplayTag(
		FName("InputTag"),
		FString("Input tag root")
	);
	Instance.InputTag_LeftMouseButton = TagManager.AddNativeGameplayTag(
		FName("InputTag.LeftMouseButton"),
		FString("Input tag for the Left Mouse Button")
	);
	Instance.InputTag_AttackTarget = TagManager.AddNativeGameplayTag(
		FName("InputTag.AttackTarget"),
		FString("Input tag for the Attack Target (Shift + LMB)")
	);
	Instance.InputTag_RightMouseButton = TagManager.AddNativeGameplayTag(
		FName("InputTag.RightMouseButton"),
		FString("Input tag for the Right Mouse Button")
	);
	Instance.InputTag_Action1 = TagManager.AddNativeGameplayTag(
		FName("InputTag.Action1"),
		FString("Input tag for Action 1")
	);
	Instance.InputTag_Action2 = TagManager.AddNativeGameplayTag(
		FName("InputTag.Action2"),
		FString("Input tag for Action 2")
	);
	Instance.InputTag_Action3 = TagManager.AddNativeGameplayTag(
		FName("InputTag.Action3"),
		FString("Input tag for Action 3")
	);
	Instance.InputTag_Action4 = TagManager.AddNativeGameplayTag(
		FName("InputTag.Action4"),
		FString("Input tag for Action 4")
	);
	Instance.InputTag_Passive1 = TagManager.AddNativeGameplayTag(
		FName("InputTag.Passive1"),
		FString("Input tag for Passive Ability 1")
	);
	Instance.InputTag_Passive2 = TagManager.AddNativeGameplayTag(
		FName("InputTag.Passive2"),
		FString("Input tag for Passive Ability 2")
	);

	// Effect Tags
	Instance.Effect_HitReact = TagManager.AddNativeGameplayTag(
		FName("Effect.HitReact"),
		FString("Tag applied when character is reacting to a hit")
	);

	// Damage Tags
	Instance.Damage = TagManager.AddNativeGameplayTag(
		FName("Damage"),
		FString("Damage")
	);
	Instance.Damage_Arcane = TagManager.AddNativeGameplayTag(
		FName("Damage.Arcane"),
		FString("Arcane Damage")
	);
	Instance.Damage_Fire = TagManager.AddNativeGameplayTag(
		FName("Damage.Fire"),
		FString("Fire Damage")
	);
	Instance.Damage_Lightning = TagManager.AddNativeGameplayTag(
		FName("Damage.Lightning"),
		FString("Lightning Damage")
	);
	Instance.Damage_Physical = TagManager.AddNativeGameplayTag(
		FName("Damage.Physical"),
		FString("Physical Damage")
	);
	Instance.DamageTypes.Add(Instance.Damage_Arcane);
	Instance.DamageTypes.Add(Instance.Damage_Fire);
	Instance.DamageTypes.Add(Instance.Damage_Lightning);
	Instance.DamageTypes.Add(Instance.Damage_Physical);

	// Resistances
	Instance.Attributes_Resistance_Arcane = TagManager.AddNativeGameplayTag(
		FName("Attributes.Resistance.Arcane"),
		FString("Arcane Damage Resistance")
	);
	Instance.Attributes_Resistance_Fire = TagManager.AddNativeGameplayTag(
		FName("Attributes.Resistance.Fire"),
		FString("Fire Damage Resistance")
	);
	Instance.Attributes_Resistance_Lightning = TagManager.AddNativeGameplayTag(
		FName("Attributes.Resistance.Lightning"),
		FString("Lightning Damage Resistance")
	);
	Instance.Attributes_Resistance_Physical = TagManager.AddNativeGameplayTag(
		FName("Attributes.Resistance.Physical"),
		FString("Physical Damage Resistance")
	);
	// Map damage type to resistance attribute
	Instance.DamageTypesToResistances.Add(Instance.Damage_Arcane, Instance.Attributes_Resistance_Arcane);
	Instance.DamageTypesToResistances.Add(Instance.Damage_Fire, Instance.Attributes_Resistance_Fire);
	Instance.DamageTypesToResistances.Add(Instance.Damage_Lightning, Instance.Attributes_Resistance_Lightning);
	Instance.DamageTypesToResistances.Add(Instance.Damage_Physical, Instance.Attributes_Resistance_Physical);

	Instance.DamageTypesToResistances.GenerateKeyArray(Instance.DamageTypes);

	Instance.Debuff = TagManager.AddNativeGameplayTag(
		FName("Debuff"),
		FString("Root tag for all debuff related tags")
	);
	Instance.Debuff_Type = TagManager.AddNativeGameplayTag(
		FName("Debuff.Type"),
		FString("Root tag for all types of debuffs")
	);
	Instance.Debuff_Type_Burn = TagManager.AddNativeGameplayTag(
		FName("Debuff.Type.Burn"),
		FString("Burns target for damage for a duration")
	);
	Instance.Debuff_Type_Shock = TagManager.AddNativeGameplayTag(
		FName("Debuff.Type.Shock"),
		FString("Shocks target for damage for a duration")
	);
	Instance.Debuff_Type_Curse = TagManager.AddNativeGameplayTag(
		FName("Debuff.Type.Curse"),
		FString("Curse target for damage for a duration")
	);
	Instance.Debuff_Type_Bleed = TagManager.AddNativeGameplayTag(
		FName("Debuff.Type.Bleed"),
		FString("Bleed target for damage for a duration")
	);
	Instance.DamageTypesToDebuffs.Add(Instance.Damage_Arcane, Instance.Debuff_Type_Curse);
	Instance.DamageTypesToDebuffs.Add(Instance.Damage_Fire, Instance.Debuff_Type_Burn);
	Instance.DamageTypesToDebuffs.Add(Instance.Damage_Lightning, Instance.Debuff_Type_Shock);
	Instance.DamageTypesToDebuffs.Add(Instance.Damage_Physical, Instance.Debuff_Type_Bleed);

	Instance.DebuffTypeToDamageType.Add(Instance.Debuff_Type_Bleed, Instance.Damage_Physical);
	Instance.DebuffTypeToDamageType.Add(Instance.Debuff_Type_Burn, Instance.Damage_Fire);
	Instance.DebuffTypeToDamageType.Add(Instance.Debuff_Type_Curse, Instance.Damage_Arcane);
	Instance.DebuffTypeToDamageType.Add(Instance.Debuff_Type_Shock, Instance.Damage_Lightning);

	Instance.Debuff_Stat = TagManager.AddNativeGameplayTag(
		FName("Debuff.Stat"),
		FString("Root tag for debuff stat tags")
	);
	Instance.Debuff_Stat_Chance = TagManager.AddNativeGameplayTag(
		FName("Debuff.Stat.Chance"),
		FString("Debuff chance")
	);
	Instance.Debuff_Stat_Damage = TagManager.AddNativeGameplayTag(
		FName("Debuff.Stat.Damage"),
		FString("Debuff Damage")
	);
	Instance.Debuff_Stat_Frequency = TagManager.AddNativeGameplayTag(
		FName("Debuff.Stat.Frequency"),
		FString("Debuff Frequency")
	);
	Instance.Debuff_Stat_Duration = TagManager.AddNativeGameplayTag(
		FName("Debuff.Stat.Duration"),
		FString("Debuff Duration")
	);
	/** Ability Tags */
	Instance.Abilities = TagManager.AddNativeGameplayTag(FName("Abilities"), FString("Abilities root tag"));
	Instance.Abilities_HitReact = TagManager.AddNativeGameplayTag(
		FName("Abilities.HitReact"),
		FString("Tag applied when character is reacting to a hit")
	);
	Instance.Abilities_Attack = TagManager.AddNativeGameplayTag(
		FName("Abilities.HitReact"),
		FString("Attack Abilities parent tag")
	);
	Instance.Abilities_Summon = TagManager.AddNativeGameplayTag(
		FName("Abilities.Summon"),
		FString("Summon Abilities parent tag")
	);
	Instance.Abilities_Fire_FireBolt = TagManager.AddNativeGameplayTag(
		FName("Abilities.Fire.FireBolt"),
		FString("FireBolt ability tag")
	);
	Instance.Abilities_Lightning_Electrocute = TagManager.AddNativeGameplayTag(
		FName("Abilities.Lightning.Electrocute"),
		FString("Electrocute ability tag")
	);
	Instance.Abilities_Passive = TagManager.AddNativeGameplayTag(
		FName("Abilities.Passive"),
		FString("Passive ability tag")
	);
	Instance.Abilities_Passive_1 = TagManager.AddNativeGameplayTag(
		FName("Abilities.Passive.1"),
		FString("Passive 1 ability tag")
	);
	Instance.Abilities_Passive_2 = TagManager.AddNativeGameplayTag(
		FName("Abilities.Passive.2"),
		FString("Passive 2 ability tag")
	);
	Instance.Abilities_Passive_3 = TagManager.AddNativeGameplayTag(
		FName("Abilities.Passive.3"),
		FString("Passive 3 ability tag")
	);
	Instance.Abilities_Status = TagManager.AddNativeGameplayTag(
		FName("Abilities.Status"),
		FString("Status Ability root")
	);
	Instance.Abilities_Status_Locked = TagManager.AddNativeGameplayTag(
		FName("Abilities.Status.Locked"),
		FString("Locked Status")
	);
	Instance.Abilities_Status_Eligible = TagManager.AddNativeGameplayTag(
		FName("Abilities.Status.Eligible"),
		FString("Eligible Status")
	);
	Instance.Abilities_Status_Unlocked = TagManager.AddNativeGameplayTag(
		FName("Abilities.Status.Unlocked"),
		FString("Unlocked Status")
	);
	Instance.Abilities_Status_Equipped = TagManager.AddNativeGameplayTag(
		FName("Abilities.Status.Equipped"),
		FString("Equipped Status")
	);
	Instance.Abilities_Type = TagManager.AddNativeGameplayTag(
		FName("Abilities.Type"),
		FString("Ability Types root")
	);
	Instance.Abilities_Type_Offensive = TagManager.AddNativeGameplayTag(
		FName("Abilities.Type.Offensive"),
		FString("Offensive Type")
	);
	Instance.Abilities_Type_Passive = TagManager.AddNativeGameplayTag(
		FName("Abilities.Type.Passive"),
		FString("Passive Type")
	);
	Instance.Abilities_Type_None = TagManager.AddNativeGameplayTag(
		FName("Abilities.Type.None"),
		FString("None Type")
	);
	/** Cooldown Tags **/
	Instance.Cooldown_Fire_FireBolt = TagManager.AddNativeGameplayTag(
		FName("Cooldown.Fire.FireBolt"),
		FString("FireBolt ability cooldown tag")
	);
	Instance.Cooldown_Lightning_Electrocute = TagManager.AddNativeGameplayTag(
		FName("Cooldown.Lightning.Electrocute"),
		FString("Electrocute ability cooldown tag")
	);


	/** Combat Socket Tags **/
	Instance.Combat_Socket_Weapon = TagManager.AddNativeGameplayTag(
		FName("Combat.Socket.Weapon"),
		FString("Weapon Socket")
	);
	Instance.Combat_Socket_RightHand = TagManager.AddNativeGameplayTag(
		FName("Combat.Socket.RightHand"),
		FString("Right Hand Socket")
	);
	Instance.Combat_Socket_LeftHand = TagManager.AddNativeGameplayTag(
		FName("Combat.Socket.LeftHand"),
		FString("Left Hand Socket")
	);
	Instance.Combat_Socket_Tail = TagManager.AddNativeGameplayTag(
		FName("Combat.Socket.Tail"),
		FString("Tail Socket")
	);

	/**
	 *Montage Attack Tags
	 */
	for (int32 i = 0; i < Instance.MontageAttackNum; i++)
	{
		Instance.Montage_Attack_Tags.Add(
			TagManager.AddNativeGameplayTag(
				FName(*FString::Printf(TEXT("Montage.Attack.%i"), i)),
				FString("Attack montage tag")
			)
		);
	}
}

bool FAuraGameplayTags::IsLeftMouseButton(const FGameplayTag& InputTag)
{
	return InputTag.MatchesTagExact(Instance.InputTag_LeftMouseButton);
}
