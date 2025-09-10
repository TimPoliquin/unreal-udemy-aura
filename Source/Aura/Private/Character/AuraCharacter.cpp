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
#include "Game/Save/OLD_AuraSaveGame.h"
#include "Game/Subsystem/AuraAIDirectorGameInstanceSubsystem.h"
#include "Game/Subsystem/AuraLevelManager.h"
#include "Game/Subsystem/Old_SaveGameManager.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Tags/AuraGameplayTags.h"
#include "UI/HUD/AuraHUD.h"
#include "Interaction/PlayerInterface.h"
#include "Player/AuraPlayerEquipmentComponent.h"
#include "Player/Progression/AuraProgressionComponent.h"


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
	EquipmentComponent = CreateDefaultSubobject<UAuraPlayerEquipmentComponent>(TEXT("Equipment Component"));
	FishingComponent = CreateDefaultSubobject<UAuraFishingComponent>(TEXT("Fishing Component"));
	FishingStatusEffectNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Fishing Status Effect"));
	FishingStatusEffectNiagaraComponent->SetupAttachment(GetRootComponent());
	FishingStatusEffectNiagaraComponent->SetAutoActivate(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Target, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Target, ECR_Ignore);
}

void AAuraCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (UAuraAIDirectorGameInstanceSubsystem* AIDirectorSubsystem = UAuraAIDirectorGameInstanceSubsystem::Get(this))
	{
		AIDirectorSubsystem->RegisterActivePlayer(this);
	}
	OnCameraReturnDelegate.BindUObject(this, &AAuraCharacter::OnCameraReturned);
}

void AAuraCharacter::BeginDestroy()
{
	Super::BeginDestroy();
	if (UAuraAIDirectorGameInstanceSubsystem* AIDirectorSubsystem = UAuraAIDirectorGameInstanceSubsystem::Get(this))
	{
		AIDirectorSubsystem->UnregisterActivePlayer(this);
	}
}

void AAuraCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAuraCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UAuraAttributeSet* AAuraCharacter::GetAuraAttributeSet() const
{
	if (const AAuraPlayerState* AuraPlayerState = GetAuraPlayerState())
	{
		return AuraPlayerState->GetAuraAttributeSet();
	}
	return nullptr;
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// Init ability actor info for the server
	InitializeAbilityActorInfo();
	if (UAuraLevelManager* LevelManager = UAuraLevelManager::Get(this); LevelManager->IsTransitioningLevels())
	{
		LevelManager->OnLevelTransitionComplete.AddWeakLambda(this, [&]()
		{
			OnLevelLoaded();
		});
	}
	else
	{
		OnLevelLoaded();
	}
}

// void AAuraCharacter::LoadProgress(const UOLD_AuraSaveGame* SaveData)
// {
// 	if (!SaveData)
// 	{
// 		return;
// 	}
// 	switch (SaveData->SaveSlotAttributeSource)
// 	{
// 	case FromDefault:
// 		InitializeDefaultAttributes();
// 		AddCharacterAbilities();
// 		break;
// 	case FromDisk:
// 		if (UAuraAbilitySystemComponent* AuraAbilitySystemComponent = GetAuraAbilitySystemComponent())
// 		{
// 			TArray<TSubclassOf<UGameplayEffect>> InitializeEffects;
// 			InitializeEffects.Add(DefaultSecondaryAttributes);
// 			InitializeEffects.Add(InitializeVitalAttributes);
// 			UAuraAbilitySystemLibrary::InitializeDefaultAttributesFromSaveData(
// 				this,
// 				AuraAbilitySystemComponent,
// 				SaveData,
// 				InitializeEffects
// 			);
// 		}
// 		break;
// 	default:
// 		UE_LOG(
// 			LogAura,
// 			Warning,
// 			TEXT("Unexpected SaveData->SaveSlotAttributeSource: [%d]"),
// 			SaveData->SaveSlotAttributeSource.GetValue()
// 		);
// 	}
// }

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	// Init ability actor info for the client
	InitializeAbilityActorInfo();
	if (APlayerController* PlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		InitializePlayerControllerHUD(PlayerController, GetPlayerState());
	}
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

UAuraAbilitySystemComponent* AAuraCharacter::GetAuraAbilitySystemComponent() const
{
	return Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent());
}

void AAuraCharacter::InitializeAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetAuraPlayerState();
	check(AuraPlayerState);
	AuraPlayerState->InitializeAbilityActorInfo();
	AbilitySystemComponent = AuraPlayerState->GetAuraAbilitySystemComponent();
	EquipmentComponent->InitializeEquipment();
	FishingComponent->SetPlayerEquipmentComponent(EquipmentComponent);
	// Broadcast Ability System Setup
	OnAbilitySystemReady(GetAuraAbilitySystemComponent());
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
			GetAuraAttributeSet()
		);
	}
}

void AAuraCharacter::OnCameraReturned()
{
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
}

void AAuraCharacter::OnLevelLoaded()
{
	if (UAuraLevelManager* LevelManager = UAuraLevelManager::Get(this))
	{
		LevelManager->OnLevelTransitionComplete.RemoveAll(this);
	}
	if (APlayerController* PlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		InitializePlayerControllerHUD(PlayerController, GetPlayerState());
	}
	AddCharacterAbilities();
}

int32 AAuraCharacter::GetCharacterLevel_Implementation() const
{
	const UAuraProgressionComponent* ProgressionComponent = UAuraProgressionComponent::Get(this);
	checkf(ProgressionComponent, TEXT("[%s] No access to progression component while trying to get character level"), *GetName());
	return ProgressionComponent->GetCharacterLevel();
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
			if (UOld_SaveGameManager* SaveGameSubsystem = UOld_SaveGameManager::Get(this))
			{
				UAuraLevelManager* LevelSubsystem = UAuraLevelManager::Get(GetWorld());
				LevelSubsystem->LoadMap(this, SaveGameSubsystem->GetInGameSaveData()->MapName);
			}
			else
			{
				UE_LOG(LogAura, Error, TEXT("[%s] Failed to get local player from aura character"), *GetName())
			}
		}
	);
	GetWorldTimerManager().SetTimer(DeathTimer, DeathTimerDelegate, DeathTime, false);
	CameraComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

USkeletalMeshComponent* AAuraCharacter::GetWeapon_Implementation() const
{
	return EquipmentComponent->GetWeapon();
}

int32 AAuraCharacter::GetXP_Implementation()
{
	const UAuraProgressionComponent* ProgressionComponent = UAuraProgressionComponent::Get(this);
	checkf(ProgressionComponent, TEXT("[%s] No access to progression component while trying to get xp"), *GetName());
	return ProgressionComponent->GetXP();
}

void AAuraCharacter::AddToXP_Implementation(int32 InXP)
{
	UAuraProgressionComponent* ProgressionComponent = UAuraProgressionComponent::Get(this);
	checkf(ProgressionComponent, TEXT("[%s] No access to progression component while trying to add xp"), *GetName());
	return ProgressionComponent->AddToXP(InXP);
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
	const UAuraProgressionComponent* ProgressionComponent = UAuraProgressionComponent::Get(this);
	checkf(ProgressionComponent, TEXT("[%s] No access to progression component while trying to FindLevelByXP"), *GetName());
	return ProgressionComponent->FindLevelByXP(InXP);
}

FAuraLevelUpRewards AAuraCharacter::GetLevelUpRewards_Implementation(const int32 InLevel) const
{
	const UAuraProgressionComponent* ProgressionComponent = UAuraProgressionComponent::Get(this);
	checkf(ProgressionComponent, TEXT("[%s] No access to progression component while trying to GetLevelUpRewards"), *GetName());
	return ProgressionComponent->GetLevelUpRewards(InLevel);
}

void AAuraCharacter::ApplyLevelUpRewards_Implementation(
	const int32 LevelIncrement,
	const FAuraLevelUpRewards& InLevelUpRewards
)
{
	UAuraProgressionComponent* ProgressionComponent = UAuraProgressionComponent::Get(this);
	checkf(ProgressionComponent, TEXT("[%s] No access to progression component while trying to ApplyLevelUpRewards"), *GetName());
	ProgressionComponent->AddAttributePoints(InLevelUpRewards.AttributePoints);
	ProgressionComponent->AddSpellPoints(InLevelUpRewards.SpellPoints);
	ProgressionComponent->AddToLevel(LevelIncrement);
}

int32 AAuraCharacter::GetAttributePoints_Implementation() const
{
	const UAuraProgressionComponent* ProgressionComponent = UAuraProgressionComponent::Get(this);
	checkf(ProgressionComponent, TEXT("[%s] No access to progression component while trying to GetAttributePoints"), *GetName());
	return ProgressionComponent->GetAttributePoints();
}

int32 AAuraCharacter::GetSpellPoints_Implementation() const
{
	const UAuraProgressionComponent* ProgressionComponent = UAuraProgressionComponent::Get(this);
	checkf(ProgressionComponent, TEXT("[%s] No access to progression component while trying to GetSpellPoints"), *GetName());
	return ProgressionComponent->GetSpellPoints();
}

void AAuraCharacter::SpendAttributePoints_Implementation(int32 SpentPoints)
{
	UAuraProgressionComponent* ProgressionComponent = UAuraProgressionComponent::Get(this);
	checkf(ProgressionComponent, TEXT("[%s] No access to progression component while trying to AddAttributePoints"), *GetName());
	return ProgressionComponent->AddAttributePoints(-1 * SpentPoints);
}

void AAuraCharacter::SpendSpellPoints_Implementation(const int32 SpentPoints)
{
	UAuraProgressionComponent* ProgressionComponent = UAuraProgressionComponent::Get(this);
	checkf(ProgressionComponent, TEXT("[%s] No access to progression component while trying to AddSpellPoints"), *GetName());
	return ProgressionComponent->AddSpellPoints(-1 * SpentPoints);
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

UAuraFishingComponent* AAuraCharacter::GetFishingComponent_Implementation() const
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
