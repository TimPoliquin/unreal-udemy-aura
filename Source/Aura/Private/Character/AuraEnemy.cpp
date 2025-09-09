// Copyright Alien Shores


#include "Character/AuraEnemy.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/LootTiers.h"
#include "Actor/Spawn/AuraSpawnBlueprintLibrary.h"
#include "AI/AuraAIController.h"
#include "Aura/Aura.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widget/AuraUserWidget.h"
#include "Actor/Spawn/TrackableInterface.h"
#include "Item/Pickup/AuraTreasurePickup.h"
#include "Item/Pickup/TieredItemInterface.h"
#include "Tags/AuraGameplayTags.h"


AAuraEnemy::AAuraEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetRenderCustomDepth(false);
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>(TEXT("AttributeSet"));
	HealthWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthWidget->SetupAttachment(GetRootComponent());
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAuraEnemy::InitializeAttributeDelegates()
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	if (UAuraUserWidget* HealthBarWidget = Cast<UAuraUserWidget>(HealthWidget->GetUserWidgetObject()))
	{
		HealthBarWidget->SetWidgetController(this);
	}
	if (const UAuraAttributeSet* AuraAttributeSet = Cast<UAuraAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).
		                        AddLambda(
			                        [&](const FOnAttributeChangeData& Data)
			                        {
				                        OnHealthChanged.Broadcast(FAuraFloatAttributeChangedPayload(GameplayTags.Attributes_Vital_Health, Data.OldValue, Data.NewValue));
			                        }
		                        );
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).
		                        AddLambda(
			                        [&](const FOnAttributeChangeData& Data)
			                        {
				                        OnMaxHealthChanged.Broadcast(FAuraFloatAttributeChangedPayload(GameplayTags.Attributes_Secondary_MaxHealth, Data.OldValue, Data.NewValue));
			                        }
		                        );
		OnHealthChanged.Broadcast(FAuraFloatAttributeChangedPayload::CreateBroadcastPayload(GameplayTags.Attributes_Vital_Health, AuraAttributeSet->GetHealth()));
		OnMaxHealthChanged.Broadcast(FAuraFloatAttributeChangedPayload::CreateBroadcastPayload(GameplayTags.Attributes_Secondary_MaxHealth, AuraAttributeSet->GetMaxHealth()));
	}
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	InitializeAbilityActorInfo();
	InitializeDefaultAttributes();
	InitializeAttributeDelegates();
	InitializeStartupAbilities();
	GetMesh()->SetCustomDepthStencilValue(HighlightCustomDepthStencilValue);
	if (Weapon)
	{
		Weapon->SetCustomDepthStencilValue(HighlightCustomDepthStencilValue);
	}
}

void AAuraEnemy::InitializeAbilityActorInfo()
{
	Super::InitializeAbilityActorInfo();
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void AAuraEnemy::InitializeDefaultAttributes()
{
	if (HasAuthority())
	{
		UAuraAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
	}
	OnAbilitySystemReady(Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent));
}

void AAuraEnemy::OnStatusShockAdded()
{
	Super::OnStatusShockAdded();
	if (AuraAIController && AuraAIController->GetBlackboardComponent())
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsStunned"), true);
	}
}

void AAuraEnemy::OnStatusShockRemoved()
{
	Super::OnStatusShockRemoved();
	if (AuraAIController && AuraAIController->GetBlackboardComponent())
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsStunned"), false);
	}
}

void AAuraEnemy::SpawnLoot()
{
	TArray<FAuraSpawnParams> SpawnParams;
	const FVector GroundLocation = UAuraSpawnBlueprintLibrary::GetGroundLocation(this);
	if (Loot)
	{
		const TArray<FLootItem> LootItems = Loot->GetLootItems();
		const TArray<FTransform> Transforms = UAuraSpawnBlueprintLibrary::GenerateSpawnLocations(GetActorLocation(), LootSpawnRadius, LootItems.Num());
		for (int32 Idx = 0; Idx < LootItems.Num() && Idx < Transforms.Num(); Idx++)
		{
			SpawnParams.Add(FAuraSpawnParams(LootItems[Idx].LootClass, Transforms[Idx]));
		}
	}
	if (!SpawnParams.IsEmpty())
	{
		SpawnLootItems(SpawnParams);
	}
	if (Treasure.IsValid() && UAuraRandRangeBlueprintLibrary::ShouldAct(TreasureSpawnRate))
	{
		AAuraTreasurePickup::SpawnTreasure(this, GroundLocation, Treasure.IsValid() ? Treasure.GetValueAtLevel(Level) : 0.f);
	}
}

AActor* AAuraEnemy::SpawnLootItem_Implementation(const FAuraSpawnParams& LootItemParams, bool bUseActorTransform)
{
	if (!LootItemParams.IsValid())
	{
		return nullptr;
	}
	const FTransform Transform = bUseActorTransform ? GetActorTransform() : LootItemParams.SpawnTransform;
	AActor* LootItem = GetWorld()->SpawnActorDeferred<AActor>(LootItemParams.SpawnClass, Transform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!LootItem)
	{
		return nullptr;
	}
	if (LootItem->Implements<UTieredItemInterface>())
	{
		ITieredItemInterface::SetItemLevel(LootItem, Level);
	}
	LootItem->FinishSpawning(Transform);
	return LootItem;
}

ECharacterClass AAuraEnemy::GetCharacterClass() const
{
	return CharacterClass;
}

void AAuraEnemy::SetCharacterClass(const ECharacterClass InCharacterClass)
{
	this->CharacterClass = InCharacterClass;
}

void AAuraEnemy::InitializeStartupAbilities()
{
	if (HasAuthority())
	{
		UAuraAbilitySystemLibrary::GrantStartupAbilities(
			this,
			AbilitySystemComponent,
			CharacterClass,
			GetCharacterLevel(this)
		);
		AddCharacterAbilities();
	}
}

void AAuraEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAuraEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (!HasAuthority())
	{
		return;
	}
	AuraAIController = CastChecked<AAuraAIController>(NewController);
	AuraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	AuraAIController->RunBehaviorTree(BehaviorTree);
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(
		FName("IsRanged"),
		CharacterClassUtils::IsRangedAttacker(CharacterClass)
	);
	AuraAIController->GetBlackboardComponent()->SetValueAsFloat(FName("TargetingRange"), TargetingRange);
	AuraAIController->GetBlackboardComponent()->SetValueAsFloat(
		FName("AttackRange_Min"),
		AttackRange - AttackRangeTolerance
	);
	AuraAIController->GetBlackboardComponent()->SetValueAsFloat(
		FName("AttackRange_Max"),
		AttackRange + AttackRangeTolerance
	);
	AuraAIController->GetBlackboardComponent()->SetValueAsFloat(
		FName("AttackWaitTime"),
		AttackWaitTime
	);
	AuraAIController->GetBlackboardComponent()->SetValueAsFloat(
		FName("AttackWaitDeviation"),
		AttackWaitDeviation
	);
}

void AAuraEnemy::HighlightActor_Implementation()
{
	GetMesh()->SetRenderCustomDepth(true);
	if (Weapon)
	{
		Weapon->SetRenderCustomDepth(true);
	}
}

void AAuraEnemy::UnHighlightActor_Implementation()
{
	GetMesh()->SetRenderCustomDepth(false);
	if (Weapon)
	{
		Weapon->SetRenderCustomDepth(false);
	}
}

int32 AAuraEnemy::GetXPReward_Implementation() const
{
	return UAuraAbilitySystemLibrary::GetXPReward(this, CharacterClass, Level);
}

USkeletalMeshComponent* AAuraEnemy::GetWeapon_Implementation() const
{
	return Weapon;
}

TArray<FName> AAuraEnemy::GetTargetTagsToIgnore_Implementation() const
{
	TArray<FName> IgnoreTargetTags;
	IgnoreTargetTags.Add(TAG_ENEMY);
	return IgnoreTargetTags;
}

void AAuraEnemy::Die()
{
	Super::Die();
	if (AuraAIController && AuraAIController->GetBlackboardComponent())
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsDead"), true);
	}
	if (HealthWidget)
	{
		HealthWidget->SetVisibility(false);
	}
	OnTrackableStopTracking.Broadcast(this);
	SetLifeSpan(LifeSpan);
	SpawnLoot();
}

FOnTrackableStopTrackingSignature& AAuraEnemy::GetStopTrackingDelegate()
{
	return OnTrackableStopTracking;
}

void AAuraEnemy::OnHitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	Super::OnHitReactTagChanged(CallbackTag, NewCount);
	if (AuraAIController && AuraAIController->GetBlackboardComponent())
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
	}
}
