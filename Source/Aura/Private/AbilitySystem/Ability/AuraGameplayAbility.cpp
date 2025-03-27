// Copyright Alien Shores


#include "AbilitySystem/Ability/AuraGameplayAbility.h"
#include "Abilities/Tasks/AbilityTask.h"

void UAuraGameplayAbility::ExecuteTask(UAbilityTask* Task) const
{
	if (UAbilitySystemComponent* AbilitySystemComponent = GetActorInfo().AbilitySystemComponent.Get())
	{
		if (Task)
		{
			Task->SetAbilitySystemComponent(AbilitySystemComponent);
			Task->ReadyForActivation();
		}
	}
}
