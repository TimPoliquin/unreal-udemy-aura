// Copyright Alien Shores


#include "Character/AuraBaseCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"
#include "Aura/Aura.h"
#include "Aura/AuraLogChannels.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Tags/AuraGameplayTags.h"

AAuraBaseCharacter::AAuraBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BurnDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>(TEXT("Burn Debuff Niagara Component"));
	BurnDebuffComponent->SetupAttachment(GetRootComponent());
	BurnDebuffComponent->DebuffTag = FAuraGameplayTags::Get().Debuff_Type_Burn;
	ShockDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>(TEXT("Shock Debuff Niagara Component"));
	ShockDebuffComponent->SetupAttachment(GetRootComponent());
	ShockDebuffComponent->DebuffTag = FAuraGameplayTags::Get().Debuff_Type_Shock;
	EffectAttachComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Effect Attach Component"));
	EffectAttachComponent->SetupAttachment(GetRootComponent());
	EffectAttachComponent->SetAbsolute(false, true, false);
	HaloOfProtectionNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>(
		TEXT("Halo of Protection Niagara Component")
	);
	HaloOfProtectionNiagaraComponent->SetupAttachment(EffectAttachComponent);
	HaloOfProtectionNiagaraComponent->PassiveSpellTag = FAuraGameplayTags::Get().Abilities_Passive_HaloOfProtection;
	LifeSiphonNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>(
		TEXT("Life Siphon Niagara Component")
	);
	LifeSiphonNiagaraComponent->SetupAttachment(EffectAttachComponent);
	LifeSiphonNiagaraComponent->PassiveSpellTag = FAuraGameplayTags::Get().Abilities_Passive_LifeSiphon;
	ManaSiphonNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>(
		TEXT("Mana Siphon Niagara Component")
	);
	ManaSiphonNiagaraComponent->SetupAttachment(EffectAttachComponent);
	ManaSiphonNiagaraComponent->PassiveSpellTag = FAuraGameplayTags::Get().Abilities_Passive_ManaSiphon;
}

void AAuraBaseCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAuraBaseCharacter, ActiveAbilityTag);
	DOREPLIFETIME(AAuraBaseCharacter, StatusEffectTags);
}


FGameplayTag AAuraBaseCharacter::GetHitReactAbilityTagByDamageType_Implementation(
	const FGameplayTag& DamageTypeTag
) const
{
	if (HitReactionsByDamageType.Contains(DamageTypeTag))
	{
		return HitReactionsByDamageType[DamageTypeTag];
	}
	return FAuraGameplayTags::Get().Effect_HitReact_Default;
}

void AAuraBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	if (!AbilitySystemComponent)
	{
		GetOnAbilitySystemRegisteredDelegate().AddUObject(this, &AAuraBaseCharacter::RegisterStatusEffectTags);
	}
	else
	{
		RegisterStatusEffectTags(AbilitySystemComponent);
	}
}


FVector AAuraBaseCharacter::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) const
{
	if (const FTaggedMontage* ActiveMontageDef = AttackMontages.FindByPredicate(
		[MontageTag](const FTaggedMontage& Item)
		{
			return Item.SocketTag.MatchesTagExact(MontageTag);
		}
	))
	{
		const FName& SocketName = ActiveMontageDef->SocketName;
		if (IsValid(Weapon) && Weapon->HasAnySockets() && Weapon->GetSocketByName(SocketName))
		{
			return Weapon->GetSocketLocation(SocketName);
		}
		return GetMesh()->GetSocketLocation(SocketName);
	}
	UE_LOG(LogTemp, Warning, TEXT("%s: No montage definition found for tag [%s]"), *GetName(), *MontageTag.ToString());
	return GetActorLocation();
}

void AAuraBaseCharacter::InitializeDefaultAttributes()
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
	ApplyEffectToSelf(InitializeVitalAttributes, 1.f);
}

void AAuraBaseCharacter::AddCharacterAbilities()
{
	if (!HasAuthority())
	{
		return;
	}
	UAuraAbilitySystemComponent* AuraAbilitySystemComponent = CastChecked<UAuraAbilitySystemComponent>(
		AbilitySystemComponent
	);
	AuraAbilitySystemComponent->AddCharacterAbilities(StartingAbilities, StartingPassiveAbilities);
	AbilitySystemComponent->RegisterGameplayTagEvent(
		FAuraGameplayTags::Get().Effect_HitReact_Default,
		EGameplayTagEventType::NewOrRemoved
	).AddUObject(
		this,
		&AAuraBaseCharacter::OnHitReactTagChanged
	);
	TArray<FGameplayTag> HitReactionKeys;
	HitReactionMontageByMontageTag.GetKeys(HitReactionKeys);
	for (const FGameplayTag& HitReactionTag : HitReactionKeys)
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(
			HitReactionTag,
			EGameplayTagEventType::NewOrRemoved
		).AddUObject(
			this,
			&AAuraBaseCharacter::OnHitReactTagChanged
		);
	}
}

void AAuraBaseCharacter::OnHitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting
		                                       ? 0
		                                       : BaseWalkSpeed;
	if (bHitReacting)
	{
		GetCharacterMovement()->DisableMovement();
	}
	else
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

bool AAuraBaseCharacter::IsShocked() const
{
	return StatusEffectTags.Contains(FAuraGameplayTags::Get().Debuff_Type_Shock);
}

bool AAuraBaseCharacter::IsBurned() const
{
	return StatusEffectTags.Contains(FAuraGameplayTags::Get().Debuff_Type_Burn);
}

AActor* AAuraBaseCharacter::GetAvatar_Implementation()
{
	return this;
}

UAnimMontage* AAuraBaseCharacter::GetHitReactMontage_Implementation(const FGameplayTag& HitReactTypeTag)
{
	if (HitReactionMontageByMontageTag.Contains(HitReactTypeTag))
	{
		return HitReactionMontageByMontageTag[HitReactTypeTag];
	}
	return HitReactMontage;
}


TArray<FTaggedMontage> AAuraBaseCharacter::GetAttackMontages_Implementation() const
{
	return AttackMontages;
}

FTaggedMontage AAuraBaseCharacter::GetTagMontageByTag_Implementation(const FGameplayTag& MontageTag)
{
	for (const FTaggedMontage& Item : AttackMontages)
	{
		if (Item.MontageTag.MatchesTagExact(MontageTag))
		{
			return Item;
		}
	}
	return FTaggedMontage();
}

void AAuraBaseCharacter::Die()
{
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	MulticastHandleDeath();
}

bool AAuraBaseCharacter::IsDead_Implementation() const
{
	return bDead;
}

UNiagaraSystem* AAuraBaseCharacter::GetBloodEffect_Implementation()
{
	return BloodEffect;
}

int32 AAuraBaseCharacter::GetXPReward_Implementation() const
{
	return 0;
}

int32 AAuraBaseCharacter::GetMinionCount_Implementation() const
{
	return MinionCount;
}

void AAuraBaseCharacter::ChangeMinionCount_Implementation(const int32 Delta)
{
	MinionCount += Delta;
}

void AAuraBaseCharacter::ApplyDeathImpulse(const FVector& DeathImpulse)
{
	GetMesh()->AddImpulse(DeathImpulse, NAME_None, true);
	if (Weapon)
	{
		Weapon->AddImpulse(DeathImpulse, NAME_None, true);
	}
}

USkeletalMeshComponent* AAuraBaseCharacter::GetWeapon_Implementation() const
{
	return Weapon;
}

void AAuraBaseCharacter::MulticastHandleDeath_Implementation()
{
	bDead = true;
	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());
	}
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Dissolve();
	OnDeathDelegate.Broadcast(this);
}

void AAuraBaseCharacter::Dissolve()
{
	Dissolve(
		GetMesh(),
		DissolveMaterialInstance,
		&AAuraBaseCharacter::StartDissolveTimeline
	);
	Dissolve(
		Weapon,
		WeaponDissolveMaterialInstance,
		&AAuraBaseCharacter::StartWeaponDissolveTimeline
	);
}

void AAuraBaseCharacter::Dissolve(
	UMeshComponent* InMesh,
	UMaterialInstance* MaterialInstance,
	void (AAuraBaseCharacter::*Callback)(UMaterialInstanceDynamic*)
)
{
	if (IsValid(InMesh) && IsValid(MaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(
			MaterialInstance,
			this
		);
		InMesh->SetMaterial(0, DynamicMaterialInstance);
		(this->*Callback)(DynamicMaterialInstance);
	}
}

void AAuraBaseCharacter::OnDebuffTypeBurnChanged(FGameplayTag GameplayTag, int StackCount)
{
	if (StackCount > 0)
	{
		StatusEffectTags.AddUnique(GameplayTag);
		OnStatusBurnAdded();
	}
	else
	{
		StatusEffectTags.Remove(GameplayTag);
		OnStatusBurnRemoved();
	}
}

void AAuraBaseCharacter::OnDebuffTypeShockChanged(FGameplayTag StunTag, int32 Count)
{
	if (Count > 0)
	{
		StatusEffectTags.AddUnique(StunTag);
		UE_LOG(LogAura, Warning, TEXT("[%s] is Stunned!"), *GetName());
		OnStatusShockAdded();
	}
	else
	{
		StatusEffectTags.Remove(StunTag);
		OnStatusShockRemoved();
	}
}


void AAuraBaseCharacter::RegisterStatusEffectTags(UAbilitySystemComponent* InAbilitySystemComponent)
{
	InAbilitySystemComponent->RegisterGameplayTagEvent(
		FAuraGameplayTags::Get().Debuff_Type_Shock,
		EGameplayTagEventType::NewOrRemoved
	).AddUObject(this, &AAuraBaseCharacter::OnDebuffTypeShockChanged);
	InAbilitySystemComponent->RegisterGameplayTagEvent(
		FAuraGameplayTags::Get().Debuff_Type_Burn,
		EGameplayTagEventType::NewOrRemoved
	).AddUObject(this, &AAuraBaseCharacter::OnDebuffTypeBurnChanged);
}

void AAuraBaseCharacter::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> Attributes, const float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(Attributes);

	FGameplayEffectContextHandle EffectContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle GameplayEffectSpec = GetAbilitySystemComponent()->MakeOutgoingSpec(
		Attributes,
		Level,
		EffectContextHandle
	);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(
		*GameplayEffectSpec.Data.Get(),
		GetAbilitySystemComponent()
	);
}


UAbilitySystemComponent* AAuraBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* AAuraBaseCharacter::GetAttributeSet() const
{
	return AttributeSet;
}
