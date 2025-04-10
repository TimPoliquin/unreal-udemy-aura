// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraItemTypes.h"
#include "GameFramework/Actor.h"
#include "AuraItemBase.generated.h"

UCLASS()
class AURA_API AAuraItemBase : public AActor
{
	GENERATED_BODY()

public:
	AAuraItemBase();

	USkeletalMeshComponent* GetMesh() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	EAuraItemType ItemType;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> MeshComponent;
};
