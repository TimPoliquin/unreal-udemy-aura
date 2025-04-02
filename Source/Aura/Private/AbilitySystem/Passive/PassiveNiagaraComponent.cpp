// Copyright Alien Shores


#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Interaction/CombatInterface.h"


// Sets default values for this component's properties
UPassiveNiagaraComponent::UPassiveNiagaraComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = false;
	// ...
}


// Called when the game starts
void UPassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();
	if (UAuraAbilitySystemComponent* AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())
	))
	{
		AuraAbilitySystemComponent->OnActivatePassiveEffectDelegate.AddUObject(
			this,
			&UPassiveNiagaraComponent::OnPassiveActivate
		);
		ActivateIfAbilityIsAlreadyActive(AuraAbilitySystemComponent);
	}
	else if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		CombatInterface->GetOnAbilitySystemRegisteredDelegate().AddLambda(
			[this](UAbilitySystemComponent* AbilitySystemComponent)
			{
				if (UAuraAbilitySystemComponent* AwaitedComponent = Cast<UAuraAbilitySystemComponent>(
					AbilitySystemComponent
				))
				{
					AwaitedComponent->OnActivatePassiveEffectDelegate.AddUObject(
						this,
						&UPassiveNiagaraComponent::OnPassiveActivate
					);
					ActivateIfAbilityIsAlreadyActive(AwaitedComponent);
				}
			}
		);
	}
}

void UPassiveNiagaraComponent::OnPassiveActivate(const FGameplayTag& AbilityTag, const bool bActivate)
{
	if (AbilityTag.MatchesTag(PassiveSpellTag))
	{
		if (bActivate && !IsActive())
		{
			Activate();
		}
		else if (!bActivate)
		{
			Deactivate();
		}
	}
}

void UPassiveNiagaraComponent::ActivateIfAbilityIsAlreadyActive(UAuraAbilitySystemComponent* AuraAbilitySystemComponent)
{
	if (AuraAbilitySystemComponent->HasFiredOnAbilitiesGivenDelegate())
	{
		if (UAuraAbilitySystemLibrary::IsAbilityEquipped(AuraAbilitySystemComponent, PassiveSpellTag))
		{
			Activate();
		}
	}
}
