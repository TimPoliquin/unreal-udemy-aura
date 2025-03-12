// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraBaseCharacter.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UNiagaraComponent;
class APlayerState;

UCLASS()
class AURA_API AAuraCharacter : public AAuraBaseCharacter, public IPlayerInterface
{
	GENERATED_BODY()

public:
	AAuraCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	// ICombatInterface
	virtual int32 GetCharacterLevel_Implementation() const override;
	virtual TArray<FName> GetTargetTagsToIgnore_Implementation() const override;

	// Player Interface
	virtual int32 GetXP_Implementation() override;
	virtual void AddToXP_Implementation(int32 InXP) override;
	virtual void LevelUp_Implementation() override;
	virtual int32 FindLevelForXP_Implementation(const int32 InXP) const override;
	virtual FAuraLevelUpRewards GetLevelUpRewards_Implementation(const int32 InLevel) const override;
	virtual void ApplyLevelUpRewards_Implementation(
		const int32 LevelIncrement,
		const FAuraLevelUpRewards& InLevelUpRewards
	) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> LevelUpSound;

	virtual void InitializeAbilityActorInfo() override;
	void InitializePlayerControllerHUD(APlayerController* PlayerController, APlayerState* PlayerState) const;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_LevelUpParticles() const;
};
