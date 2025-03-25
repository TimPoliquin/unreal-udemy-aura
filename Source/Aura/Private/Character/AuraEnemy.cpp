// Copyright Alien Shores


#include "Character/AuraEnemy.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AI/AuraAIController.h"
#include "Aura/Aura.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widget/AuraUserWidget.h"


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
}

void AAuraEnemy::InitializeAttributeDelegates()
{
	if (UAuraUserWidget* HealthBarWidget = Cast<UAuraUserWidget>(HealthWidget->GetUserWidgetObject()))
	{
		HealthBarWidget->SetWidgetController(this);
	}
	if (const UAuraAttributeSet* AuraAttributeSet = Cast<UAuraAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).
		                        AddLambda(
			                        [this](const FOnAttributeChangeData& Data)
			                        {
				                        OnHealthChanged.Broadcast(Data.NewValue);
			                        }
		                        );
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).
		                        AddLambda(
			                        [this](const FOnAttributeChangeData& Data)
			                        {
				                        OnMaxHealthChanged.Broadcast(Data.NewValue);
			                        }
		                        );
		OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
		OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
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

void AAuraEnemy::InitializeDefaultAttributes() const
{
	if (HasAuthority())
	{
		UAuraAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
	}
	GetOnAbilitySystemRegisteredDelegate().Broadcast(AbilitySystemComponent);
}

void AAuraEnemy::InitializeStartupAbilities() const
{
	if (HasAuthority())
	{
		UAuraAbilitySystemLibrary::GrantStartupAbilities(
			this,
			AbilitySystemComponent,
			CharacterClass,
			GetCharacterLevel(this)
		);
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

void AAuraEnemy::HighlightActor()
{
	GetMesh()->SetRenderCustomDepth(true);
	if (Weapon)
	{
		Weapon->SetRenderCustomDepth(true);
	}
}

void AAuraEnemy::UnHighlightActor()
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
	SetLifeSpan(LifeSpan);
}

void AAuraEnemy::OnHitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	Super::OnHitReactTagChanged(CallbackTag, NewCount);
	if (AuraAIController && AuraAIController->GetBlackboardComponent())
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
	}
}
