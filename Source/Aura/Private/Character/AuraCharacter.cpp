// Copyright Alien Shores


#include "Character/AuraCharacter.h"
#include "AbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HUD/AuraHUD.h"


AAuraCharacter::AAuraCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 400.f, 0);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Level Up Effect"));
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
	LevelUpNiagaraComponent->SetAutoActivate(false);
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->SetUsingAbsoluteRotation(true);
	SpringArmComponent->bDoCollisionTest = false;
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;
}

void AAuraCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAuraCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// Init ability actor info for the server
	InitializeAbilityActorInfo();
	// Add startup abilities (server-side)
	AddCharacterAbilities();
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	// Init ability actor info for the client
	InitializeAbilityActorInfo();
}

void AAuraCharacter::InitializeAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();
	if (APlayerController* PlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		InitializePlayerControllerHUD(PlayerController, AuraPlayerState);
	}
	InitializeDefaultAttributes();
}

void AAuraCharacter::InitializePlayerControllerHUD(
	APlayerController* InPlayerController,
	APlayerState* InPlayerState
) const
{
	if (AAuraHUD* HUD = Cast<AAuraHUD>(InPlayerController->GetHUD()))
	{
		HUD->InitializeWidgets(
			InPlayerController,
			InPlayerState,
			AbilitySystemComponent,
			AttributeSet
		);
	}
}

int32 AAuraCharacter::GetCharacterLevel_Implementation() const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetCharacterLevel();
}

TArray<FName> AAuraCharacter::GetTargetTagsToIgnore_Implementation() const
{
	TArray<FName> IgnoreTags;
	IgnoreTags.Add(TAG_PLAYER);
	return IgnoreTags;
}

int32 AAuraCharacter::GetXP_Implementation()
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetXP();
}

void AAuraCharacter::AddToXP_Implementation(int32 InXP)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToXP(InXP);
}

void AAuraCharacter::LevelUp_Implementation()
{
	Multicast_LevelUpParticles();
}

void AAuraCharacter::Multicast_LevelUpParticles_Implementation() const
{
	if (IsValid(LevelUpNiagaraComponent))
	{
		const FVector CameraLocation = CameraComponent->GetComponentLocation();
		const FVector EffectLocation = LevelUpNiagaraComponent->GetComponentLocation();
		const FRotator ToCameraRotation = (CameraLocation - EffectLocation).Rotation();
		LevelUpNiagaraComponent->Activate(true);
		LevelUpNiagaraComponent->SetWorldRotation(ToCameraRotation);
	}
	if (LevelUpSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, LevelUpSound, GetActorLocation(), GetActorRotation());
	}
}

int32 AAuraCharacter::FindLevelForXP_Implementation(const int32 InXP) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->FindLevelByXP(InXP);
}

FAuraLevelUpRewards AAuraCharacter::GetLevelUpRewards_Implementation(const int32 InLevel) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetLevelUpRewards(InLevel);
}

void AAuraCharacter::ApplyLevelUpRewards_Implementation(
	const int32 LevelIncrement,
	const FAuraLevelUpRewards& InLevelUpRewards
)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddAttributePoints(InLevelUpRewards.AttributePoints);
	AuraPlayerState->AddSpellPoints(InLevelUpRewards.SpellPoints);
	AuraPlayerState->AddToLevel(LevelIncrement);

	if (UAuraAbilitySystemComponent* AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(
		GetAbilitySystemComponent()
	))
	{
		AuraAbilitySystemComponent->ServerUpdateAbilityStatuses(AuraPlayerState->GetCharacterLevel());
	}
}

int32 AAuraCharacter::GetAttributePoints_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetAttributePoints();
}

int32 AAuraCharacter::GetSpellPoints_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetSpellPoints();
}

void AAuraCharacter::SpendAttributePoints_Implementation(int32 SpentPoints)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddAttributePoints(-1 * SpentPoints);
}

void AAuraCharacter::SpendSpellPoints_Implementation(const int32 SpentPoints)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddSpellPoints(-1 * SpentPoints);
}
