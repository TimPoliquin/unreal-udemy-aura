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
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "Aura/AuraLogChannels.h"
#include "Camera/AuraCameraComponent.h"
#include "Camera/CameraComponent.h"
#include "Fishing/AuraFishingComponent.h"
#include "Game/AuraGameModeBase.h"
#include "Game/AuraSaveGame.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayerInventoryComponent.h"
#include "Tags/AuraGameplayTags.h"
#include "UI/HUD/AuraHUD.h"
#include "Interaction/PlayerInterface.h"


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
	CameraComponent = CreateDefaultSubobject<UAuraCameraComponent>(TEXT("Camera Component"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;
	PlayerInventoryComponent = CreateDefaultSubobject<UPlayerInventoryComponent>(TEXT("Player Inventory"));
	FishingComponent = CreateDefaultSubobject<UAuraFishingComponent>(TEXT("Fishing Component"));
	FishingComponent->SetPlayerInventoryComponent(PlayerInventoryComponent);
	FishingStatusEffectNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Fishing Status Effect"));
	FishingStatusEffectNiagaraComponent->SetupAttachment(GetRootComponent());
	FishingStatusEffectNiagaraComponent->SetAutoActivate(false);
}

void AAuraCharacter::BeginPlay()
{
	Super::BeginPlay();
	OnCameraReturnDelegate.BindUObject(this, &AAuraCharacter::OnCameraReturned);
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
	LoadProgress();
	AAuraGameModeBase* GameMode = AAuraGameModeBase::GetAuraGameMode(this);
	GameMode->LoadWorldState(GetWorld());
}


void AAuraCharacter::LoadProgress()
{
	if (const AAuraGameModeBase* GameMode = AAuraGameModeBase::GetAuraGameMode(this))
	{
		const UAuraSaveGame* SaveData = GameMode->GetInGameSaveData();
		if (!SaveData)
		{
			return;
		}
		switch (SaveData->SaveSlotAttributeSource)
		{
		case FromDefault:
			InitializeDefaultAttributes();
			AddCharacterAbilities();
			break;
		case FromDisk:
			if (UAuraAttributeSet* AuraAttributeSet = GetAuraAttributeSet())
			{
				AuraAttributeSet->FromSaveData(SaveData);
			}
			if (AAuraPlayerState* AuraPlayerState = GetAuraPlayerState())
			{
				AuraPlayerState->FromSaveData(SaveData);
			}
			if (UAuraAbilitySystemComponent* AuraAbilitySystemComponent = GetAuraAbilitySystemComponent())
			{
				AuraAbilitySystemComponent->FromSaveData(SaveData);
			}
			if (PlayerInventoryComponent)
			{
				PlayerInventoryComponent->FromSaveData(SaveData);
			}
			break;
		default:
			UE_LOG(
				LogAura,
				Warning,
				TEXT("Unexpected SaveData->SaveSlotAttributeSource: [%d]"),
				SaveData->SaveSlotAttributeSource.GetValue()
			);
		}
	}
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	// Init ability actor info for the client
	InitializeAbilityActorInfo();
}

void AAuraCharacter::OnRep_ActiveAbilityTag()
{
	Super::OnRep_ActiveAbilityTag();
}

void AAuraCharacter::OnRep_StatusEffectTags()
{
	Super::OnRep_StatusEffectTags();
	if (UAuraAbilitySystemComponent* AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(
		AbilitySystemComponent
	))
	{
		const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
		FGameplayTagContainer BlockedTags;
		BlockedTags.AddTag(GameplayTags.Player_Block_CursorTrace);
		BlockedTags.AddTag(GameplayTags.Player_Block_Movement);
		BlockedTags.AddTag(GameplayTags.Player_Block_Ability_Offensive);
		BlockedTags.AddTag(GameplayTags.Player_Block_Interaction);
		if (IsShocked())
		{
			AuraAbilitySystemComponent->AddLooseGameplayTags(BlockedTags);
			ShockDebuffComponent->Activate();
		}
		else
		{
			AuraAbilitySystemComponent->RemoveLooseGameplayTags(BlockedTags);
			ShockDebuffComponent->Deactivate();
		}
		if (IsBurned())
		{
			BurnDebuffComponent->Activate();
		}
		else
		{
			BurnDebuffComponent->Deactivate();
		}
	}
}

AAuraPlayerState* AAuraCharacter::GetAuraPlayerState() const
{
	return Cast<AAuraPlayerState>(GetPlayerState());
}

UAuraAttributeSet* AAuraCharacter::GetAuraAttributeSet() const
{
	return Cast<UAuraAttributeSet>(GetAttributeSet());
}

UAuraAbilitySystemComponent* AAuraCharacter::GetAuraAbilitySystemComponent() const
{
	return Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent());
}

void AAuraCharacter::InitializeAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetAuraPlayerState();
	check(AuraPlayerState);
	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();
	if (APlayerController* PlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		InitializePlayerControllerHUD(PlayerController, AuraPlayerState);
	}
	// Broadcast Ability System Setup
	GetOnAbilitySystemRegisteredDelegate().Broadcast(AbilitySystemComponent);
}

void AAuraCharacter::InitializePlayerControllerHUD(
	APlayerController* InPlayerController,
	APlayerState* InPlayerState
)
{
	if (AAuraHUD* HUD = Cast<AAuraHUD>(InPlayerController->GetHUD()))
	{
		HUD->InitializeWidgets(
			this,
			InPlayerController,
			InPlayerState,
			AbilitySystemComponent,
			AttributeSet
		);
	}
}

void AAuraCharacter::OnCameraReturned()
{
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
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

void AAuraCharacter::Die()
{
	Super::Die();
	FTimerDelegate DeathTimerDelegate;
	DeathTimerDelegate.BindLambda(
		[this]()
		{
			if (AAuraGameModeBase* GameMode = AAuraGameModeBase::GetAuraGameMode(this))
			{
				GameMode->PlayerDied(this);
			}
		}
	);
	GetWorldTimerManager().SetTimer(DeathTimer, DeathTimerDelegate, DeathTime, false);
	CameraComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

USkeletalMeshComponent* AAuraCharacter::GetWeapon_Implementation() const
{
	return PlayerInventoryComponent->GetWeapon();
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

void AAuraCharacter::ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial)
{
	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		AuraPlayerController->ShowMagicCircle(DecalMaterial);
	}
}

void AAuraCharacter::HideMagicCircle_Implementation()
{
	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		AuraPlayerController->HideMagicCircle();
	}
}

void AAuraCharacter::SaveProgress_Implementation(const FName& CheckpointTag)
{
	AAuraGameModeBase* GameMode = AAuraGameModeBase::GetAuraGameMode(this);
	UAuraSaveGame* SaveData = GameMode
		                          ? GameMode->GetInGameSaveData()
		                          : nullptr;
	if (SaveData)
	{
		if (const AAuraPlayerState* AuraPlayerState = GetAuraPlayerState())
		{
			AuraPlayerState->ToSaveData(SaveData);
		}
		else
		{
			UE_LOG(LogAura, Error, TEXT("SAVE ERROR: No player state!"))
		}
		if (const UAuraAttributeSet* AuraAttributeSet = GetAuraAttributeSet())
		{
			AuraAttributeSet->ToSaveData(SaveData);
		}
		else
		{
			UE_LOG(LogAura, Error, TEXT("SAVE ERROR: No attribute set!"))
		}
		if (UAuraAbilitySystemComponent* AuraAbilitySystemComponent = GetAuraAbilitySystemComponent())
		{
			AuraAbilitySystemComponent->ToSaveData(SaveData);
		}
		else
		{
			UE_LOG(LogAura, Error, TEXT("SAVE ERROR: No AuraAbilitySystemComponent set!"))
		}
		if (PlayerInventoryComponent)
		{
			PlayerInventoryComponent->ToSaveData(SaveData);
		}
		SaveData->SaveSlotAttributeSource = FromDisk;
		SaveData->PlayerStartTag = CheckpointTag;
		GameMode->SaveInGameProgressData(SaveData);
	}
}

void AAuraCharacter::MoveCameraToPoint_Implementation(
	const FVector& Destination,
	const FVector& Direction,
	UCurveFloat* AnimationCurve
)
{
	DesiredCameraForwardVector = CameraComponent->GetForwardVector();
	CameraComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	CameraComponent->MoveToLocation(Destination, Direction, AnimationCurve);
}

void AAuraCharacter::MoveCameraToPointWithCallback(
	const FVector& Destination,
	const FVector& Direction,
	UCurveFloat* AnimationCurve,
	FOnCameraMoveFinishedSignature& OnCameraMoveFinishedSignature
)
{
	DesiredCameraForwardVector = CameraComponent->GetForwardVector();
	CameraComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	CameraComponent->MoveToLocation(Destination, Direction, AnimationCurve, &OnCameraMoveFinishedSignature);
}

void AAuraCharacter::ReturnCamera_Implementation(
	UCurveFloat* AnimationCurve
)
{
	CameraComponent->AttachToComponent(
		SpringArmComponent,
		FAttachmentTransformRules::KeepWorldTransform,
		USpringArmComponent::SocketName
	);
	CameraComponent->MoveToLocation(
		SpringArmComponent->GetSocketTransform(USpringArmComponent::SocketName).GetLocation(),
		DesiredCameraForwardVector,
		AnimationCurve,
		&OnCameraReturnDelegate
	);
}

UPlayerInventoryComponent* AAuraCharacter::GetInventoryComponent_Implementation() const
{
	return PlayerInventoryComponent;
}

TScriptInterface<IFishingComponentInterface> AAuraCharacter::GetFishingComponent_Implementation() const
{
	return FishingComponent;
}

void AAuraCharacter::ShowFishingStatusEffect_Implementation(UNiagaraSystem* EffectSystem)
{
	if (EffectSystem)
	{
		FishingStatusEffectNiagaraComponent->SetAsset(EffectSystem);
		FishingStatusEffectNiagaraComponent->Activate(true);
	}
	else
	{
		FishingStatusEffectNiagaraComponent->SetAsset(nullptr);
		FishingStatusEffectNiagaraComponent->DeactivateImmediate();
	}
}
