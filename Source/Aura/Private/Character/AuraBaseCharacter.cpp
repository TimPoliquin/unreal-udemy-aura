// Copyright Alien Shores


#include "Character/AuraBaseCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"

AAuraBaseCharacter::AAuraBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
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
}

FVector AAuraBaseCharacter::GetCombatSocketLocation() const
{
	return Weapon->GetSocketLocation(WeaponTipSocketName);
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
	AuraAbilitySystemComponent->AddCharacterAbilities(StartingAbilities);
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
