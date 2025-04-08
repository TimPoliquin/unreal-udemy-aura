// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraBaseCharacter.h"
#include "Interaction/PlayerInterface.h"
#include "Player/AuraPlayerState.h"
#include "AuraCharacter.generated.h"

class UAuraCameraComponent;
class UAuraAbilitySystemComponent;
class UAuraAttributeSet;
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
	virtual void OnRep_ActiveAbilityTag() override;
	virtual void OnRep_StatusEffectTags() override;
	AAuraPlayerState* GetAuraPlayerState() const;
	UAuraAttributeSet* GetAuraAttributeSet() const;
	UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent() const;
	// ICombatInterface
	virtual int32 GetCharacterLevel_Implementation() const override;
	virtual TArray<FName> GetTargetTagsToIgnore_Implementation() const override;
	virtual void Die() override;

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
	virtual int32 GetAttributePoints_Implementation() const override;
	virtual int32 GetSpellPoints_Implementation() const override;
	virtual void SpendAttributePoints_Implementation(int32 SpentPoints) override;
	virtual void SpendSpellPoints_Implementation(int32 SpentPoints) override;
	virtual void ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial = nullptr) override;
	virtual void HideMagicCircle_Implementation() override;
	virtual void SaveProgress_Implementation(const FName& CheckpointTag) override;
	virtual void MoveCameraToPoint_Implementation(
		const FVector& Destination,
		const FVector& Direction,
		UCurveFloat* AnimationCurve
	) override;
	virtual void ReturnCamera_Implementation(
		UCurveFloat* AnimationCurve
	) override;

protected:
	virtual void BeginPlay() override;
	void LoadProgress();

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAuraCameraComponent> CameraComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> LevelUpSound;
	UPROPERTY(EditDefaultsOnly)
	float DeathTime = 5.f;
	UPROPERTY()
	FTimerHandle DeathTimer;

	virtual void InitializeAbilityActorInfo() override;
	void InitializePlayerControllerHUD(APlayerController* InPlayerController, APlayerState* InPlayerState) const;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_LevelUpParticles() const;
};
