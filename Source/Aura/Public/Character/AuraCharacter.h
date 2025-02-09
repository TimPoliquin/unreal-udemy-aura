// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraBaseCharacter.h"
#include "AuraCharacter.generated.h"

class APlayerState;

UCLASS()
class AURA_API AAuraCharacter : public AAuraBaseCharacter
{
	GENERATED_BODY()

public:
	AAuraCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	// ICombatInterface
	virtual int32 GetCharacterLevel() const override;

protected:
	virtual void BeginPlay() override;

private:
	virtual void InitializeAbilityActorInfo() override;
	void InitializePlayerControllerHUD(APlayerController* PlayerController, APlayerState* PlayerState) const;
};
