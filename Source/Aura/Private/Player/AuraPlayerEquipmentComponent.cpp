// Copyright Alien Shores


#include "Player/AuraPlayerEquipmentComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Aura/AuraLogChannels.h"
#include "Game/Save/AuraSaveGameBlueprintFunctionLibrary.h"
#include "Game/Save/OLD_AuraSaveGame.h"
#include "GameFramework/Character.h"
#include "Item/AuraItemBlueprintLibrary.h"
#include "Item/Equipment/AuraEquipmentBase.h"
#include "Item/Equipment/AuraFishingRod.h"
#include "Tags/AuraGameplayTags.h"

UAuraPlayerEquipmentComponent::UAuraPlayerEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SaveID = UAuraSaveGameBlueprintFunctionLibrary::GenerateSaveID(this);
	SetIsReplicatedByDefault(true);
}

TArray<uint8> UAuraPlayerEquipmentComponent::SaveData_Implementation()
{
	return SerializeComponentData();
}

bool UAuraPlayerEquipmentComponent::LoadData_Implementation(const TArray<uint8>& Data)
{
	return DeserializeComponentData(Data);
}

FString UAuraPlayerEquipmentComponent::GetSaveID_Implementation() const
{
	return SaveID;
}

void UAuraPlayerEquipmentComponent::InitializeEquipment()
{
	switch (EquipmentUseMode)
	{
	case EAuraEquipmentUseMode::Tool:
		UseTool();
		break;
	case EAuraEquipmentUseMode::Weapon:
		UseWeapon();
		break;
	default:
		UseNothing();
		break;
	}
}

bool UAuraPlayerEquipmentComponent::HasToolEquipped(const FGameplayTag& ItemType) const
{
	return EquipmentUseMode == EAuraEquipmentUseMode::Tool && EquipmentSlots[EAuraEquipmentSlot::Tool] == ItemType;
}

bool UAuraPlayerEquipmentComponent::IsUsingTool() const
{
	return EquipmentUseMode == EAuraEquipmentUseMode::Tool;
}

bool UAuraPlayerEquipmentComponent::IsUsingWeapon() const
{
	return EquipmentUseMode == EAuraEquipmentUseMode::Weapon;
}

void UAuraPlayerEquipmentComponent::UseEquipment_Implementation(const EAuraEquipmentSlot Slot)
{
	switch (Slot)
	{
	case EAuraEquipmentSlot::Tool:
		UseTool();
		break;
	case EAuraEquipmentSlot::Weapon:
		UseWeapon();
		break;
	default:
		UseNothing();
		break;
	}
}

void UAuraPlayerEquipmentComponent::UseTool()
{
	if (IsValid(Weapon))
	{
		Weapon->SetHidden(true);
		Weapon->GetMesh()->SetVisibility(false);
		Weapon->UnEquip(GetOwner());
	}
	EquipmentUseMode = EAuraEquipmentUseMode::Tool;
	if (!IsValid(Tool))
	{
		Tool = SpawnEquipment(EAuraEquipmentSlot::Tool);
	}
	if (Tool)
	{
		Tool->SetHidden(false);
		Tool->GetMesh()->SetVisibility(true);
		Tool->Equip(GetOwner());
	}
	OnUseTool.Broadcast();
}

void UAuraPlayerEquipmentComponent::UseWeapon()
{
	if (IsValid(Tool))
	{
		Tool->SetHidden(true);
		Tool->GetMesh()->SetVisibility(false);
		Tool->UnEquip(GetOwner());
	}
	EquipmentUseMode = EAuraEquipmentUseMode::Weapon;
	if (!IsValid(Weapon))
	{
		Weapon = SpawnEquipment(EAuraEquipmentSlot::Weapon);
	}
	if (Weapon)
	{
		Weapon->SetHidden(false);
		Weapon->GetMesh()->SetVisibility(true);
		Weapon->Equip(GetOwner());
	}
	OnUseWeapon.Broadcast();
}

void UAuraPlayerEquipmentComponent::UseNothing()
{
	if (Weapon)
	{
		Weapon->SetHidden(true);
	}
	if (Tool)
	{
		Tool->SetHidden(true);
	}
	EquipmentUseMode = EAuraEquipmentUseMode::None;
	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
		GetOwner()
	))
	{
		AbilitySystemComponent->RemoveReplicatedLooseGameplayTag(FAuraGameplayTags::Get().Player_Equipped_Tool);
		AbilitySystemComponent->RemoveReplicatedLooseGameplayTag(FAuraGameplayTags::Get().Player_Equipped_Weapon);
	}
}

void UAuraPlayerEquipmentComponent::Equip(const EAuraEquipmentSlot& Slot, const FGameplayTag& ItemType)
{
	if (EquipmentSlots.Contains(Slot))
	{
		EquipmentSlots[Slot] = ItemType;
	}
	else
	{
		EquipmentSlots.Add(Slot, ItemType);
	}
}

USkeletalMeshComponent* UAuraPlayerEquipmentComponent::GetWeapon() const
{
	if (EquipmentUseMode == EAuraEquipmentUseMode::Weapon && EquipmentSlots.Contains(EAuraEquipmentSlot::Weapon) &&
		IsValid(Weapon))
	{
		return Weapon->GetMesh();
	}
	return nullptr;
}

FGameplayTag UAuraPlayerEquipmentComponent::GetToolType() const
{
	if (EquipmentSlots.Contains(EAuraEquipmentSlot::Tool))
	{
		return EquipmentSlots[EAuraEquipmentSlot::Tool];
	}
	return FGameplayTag::EmptyTag;
}

FGameplayTag UAuraPlayerEquipmentComponent::GetWeaponType() const
{
	if (EquipmentSlots.Contains(EAuraEquipmentSlot::Weapon))
	{
		return EquipmentSlots[EAuraEquipmentSlot::Weapon];
	}
	return FGameplayTag::EmptyTag;
}

AAuraFishingRod* UAuraPlayerEquipmentComponent::GetFishingRod() const
{
	if (IsValid(Tool) && Tool->GetItemType() == FAuraGameplayTags::Get().Item_Type_Equipment_FishingRod)
	{
		return Cast<AAuraFishingRod>(Tool);
	}
	return nullptr;
}

FGameplayTag UAuraPlayerEquipmentComponent::GetEquippedItem(const EAuraEquipmentSlot Slot) const
{
	if (EquipmentSlots.Contains(Slot))
	{
		return EquipmentSlots[Slot];
	}
	return FGameplayTag::EmptyTag;
}

EAuraEquipmentUseMode UAuraPlayerEquipmentComponent::GetEquipmentUseMode() const
{
	return EquipmentUseMode;
}

void UAuraPlayerEquipmentComponent::PlayEquipAnimation(const EAuraEquipmentSlot Slot) const
{
	const FGameplayTag& ItemType = GetEquippedItem(Slot);
	OnEquipmentAnimationRequest.Broadcast(FAuraEquipmentDelegatePayload(Slot, ItemType));
}

void UAuraPlayerEquipmentComponent::EquipAnimationCompleted_Implementation(const EAuraEquipmentSlot Slot)
{
	OnEquipmentAnimationCompleteDelegate.Broadcast(FAuraEquipmentDelegatePayload(Slot, EquipmentSlots[Slot]));
}


void UAuraPlayerEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
}

USkeletalMeshComponent* UAuraPlayerEquipmentComponent::GetCharacterMesh() const
{
	return Cast<ACharacter>(GetOwner())->GetMesh();
}

AAuraEquipmentBase* UAuraPlayerEquipmentComponent::SpawnEquipment(const EAuraEquipmentSlot& Slot)
{
	if (!EquipmentSlots.Contains(Slot))
	{
		return nullptr;
	}
	USkeletalMeshComponent* CharacterMesh = GetCharacterMesh();
	if (!IsValid(CharacterMesh))
	{
		UE_LOG(LogAura, Warning, TEXT("[%s] No character mesh set for player %s"), *GetName(), *GetOwner()->GetName())
		return nullptr;
	}
	const FName SocketName = EquipmentSocketNames[Slot];
	const FVector SocketLocation = CharacterMesh->GetSocketLocation(SocketName);
	const FRotator SocketRotation = CharacterMesh->GetSocketRotation(SocketName);

	const FAuraItemDefinition ItemDefinition = UAuraItemBlueprintLibrary::GetItemDefinitionByItemType(
		this,
		EquipmentSlots[Slot]
	);
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = CharacterMesh->GetOwner();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AAuraEquipmentBase* Equipment = GetWorld()->SpawnActor<AAuraEquipmentBase>(
		ItemDefinition.EquipmentClass,
		SocketLocation,
		SocketRotation,
		SpawnParameters
	);
	if (!Equipment)
	{
		UE_LOG(LogAura, Error, TEXT("[%s][%s] Failed to spawn equipment for slot [%s]. Check item configuration."), *GetOwner()->GetName(), *GetName(), *ItemDefinition.ItemType.ToString());
		return nullptr;
	}
	Equipment->AttachToComponent(
		CharacterMesh,
		FAttachmentTransformRules::KeepWorldTransform,
		SocketName
	);
	return Equipment;
}

TArray<uint8> UAuraPlayerEquipmentComponent::SerializeComponentData() const
{
	TArray<uint8> Data;
	FMemoryWriter Writer(Data);

	// Create save data struct
	FAuraPlayerEquipmentComponentSaveData SaveData;
	SaveData.EquipmentUseMode = EquipmentUseMode;
	SaveData.EquipmentSlots = EquipmentSlots;

	// Serialize the struct
	Writer << SaveData.EquipmentUseMode;
	Writer << SaveData.EquipmentSlots;

	return Data;
}

bool UAuraPlayerEquipmentComponent::DeserializeComponentData(const TArray<uint8>& Data)
{
	if (Data.Num() == 0)
	{
		return false;
	}

	FMemoryReader Reader(Data);

	try
	{
		Reader << EquipmentUseMode;
		Reader << EquipmentSlots;

		return true;
	}
	catch (...)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s:%s] Failed to deserialize equipment data"), *GetOwner()->GetName(), *GetName());
		return false;
	}
}
