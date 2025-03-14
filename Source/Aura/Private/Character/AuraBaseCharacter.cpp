// Copyright Alien Shores


#include "Character/AuraBaseCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Tags/AuraGameplayTags.h"
#include "Utils/ArrayUtils.h"

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
}


void AAuraBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
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

void AAuraBaseCharacter::InitializeDefaultAttributes() const
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
		FAuraGameplayTags::Get().Effect_HitReact,
		EGameplayTagEventType::NewOrRemoved
	).AddUObject(
		this,
		&AAuraBaseCharacter::OnHitReactTagChanged
	);
}

void AAuraBaseCharacter::OnHitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting
		                                       ? 0
		                                       : BaseWalkSpeed;
}

AActor* AAuraBaseCharacter::GetAvatar_Implementation()
{
	return this;
}


UAnimMontage* AAuraBaseCharacter::GetHitReactMontage_Implementation()
{
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
