// Copyright Alien Shores


#include "Tags/AuraGameplayTags.h"
#include "GameplayTagsManager.h"

FAuraGameplayTags FAuraGameplayTags::Instance;

void FAuraGameplayTags::InitializeNativeGameplayTags()
{
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
}
