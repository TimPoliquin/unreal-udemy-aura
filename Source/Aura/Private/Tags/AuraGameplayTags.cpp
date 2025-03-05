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

	// Input Tags
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

	/** Ability Tags */
	Instance.Abilities_Attack = TagManager.AddNativeGameplayTag(
		FName("Abilities.Attack"),
		FString("Attack Abilities parent tag")
	);
	Instance.Abilities_Summon = TagManager.AddNativeGameplayTag(
		FName("Abilities.Summon"),
		FString("Summon Abilities parent tag")
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
