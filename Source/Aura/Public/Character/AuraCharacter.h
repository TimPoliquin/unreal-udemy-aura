// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraBaseCharacter.h"
#include "Camera/AuraCameraComponent.h"
#include "Interaction/FishingInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Player/AuraPlayerState.h"
#include "AuraCharacter.generated.h"

class UPlayerInventoryComponent;
class UAuraCameraComponent;
class UAuraAbilitySystemComponent;
class UAuraAttributeSet;
class USpringArmComponent;
class UCameraComponent;
class UNiagaraComponent;
class APlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnAnimationCompleteSignature,
	const EAuraEquipmentSlot&,
	Slot
);

UCLASS()
class AURA_API AAuraCharacter : public AAuraBaseCharacter, public IPlayerInterface, public IFishingInterface
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
	virtual USkeletalMeshComponent* GetWeapon_Implementation() const override;

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
	virtual void MoveCameraToPointWithCallback(
		const FVector& Destination,
		const FVector& Direction,
		UCurveFloat* AnimationCurve,
		FOnCameraMoveFinishedSignature& OnCameraMoveFinishedSignature
	) override;
	virtual void ReturnCamera_Implementation(
		UCurveFloat* AnimationCurve
	) override;

	/** FishingInterface Start */
	virtual bool HasFishingRod_Implementation() override;
	virtual bool HasFishingRodEquipped_Implementation() override;
	virtual void EquipFishingRod_Implementation() override;
	virtual void CastFishingRod_Implementation(const FVector& FishingLocation) override;
	virtual FOnFishingRodEquippedSignature& GetOnFishingRodEquippedDelegate() override;
	virtual FOnFishingRodCastSignature& GetOnFishingRodCastDelegate() override;
	virtual void EndFishing_Implementation() override;

	UFUNCTION(BlueprintImplementableEvent)
	void PlayEquipAnimation(const EAuraEquipmentSlot& Slot);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayFishingCastAnimation();
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnFishingRodEquippedSignature OnFishingRodEquippedDelegate;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnFishingRodCastSignature OnFishingRodCastDelegate;
	/** FishingInterface End */

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnAnimationCompleteSignature OnEquipAnimationCompleteDelegate;

protected:
	virtual void BeginPlay() override;
	void LoadProgress();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UPlayerInventoryComponent> PlayerInventoryComponent;

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
	FOnCameraMoveFinishedSignature OnCameraReturnDelegate;

	virtual void InitializeAbilityActorInfo() override;
	void InitializePlayerControllerHUD(APlayerController* InPlayerController, APlayerState* InPlayerState) const;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_LevelUpParticles() const;
	UFUNCTION()
	void OnCameraReturned();
	UFUNCTION()
	void OnEquipmentAnimationComplete(const EAuraEquipmentSlot& Slot);

	FVector DesiredCameraForwardVector;
};
