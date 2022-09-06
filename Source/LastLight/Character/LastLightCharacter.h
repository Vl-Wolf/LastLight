// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LastLight/FuncLibrary/Types.h"
#include "LastLight/Weapons/WeaponDefault.h"
#include "LastLight/Character/LastLightCharHealthComponent.h"
#include "LastLight/Character/LastLightInventoryComponent.h"
#include "LastLightCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
class ALastLightCharacter : public ACharacter
{
	GENERATED_BODY()


public:
	ALastLightCharacter();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		USkeletalMeshComponent* CharacterHead;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
		ULastLightCharHealthComponent* HealthComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Death")
		TArray<UAnimMontage*> DeadsAnim;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
		ULastLightInventoryComponent* InventoryComponent;

protected:
	
	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaSeconds) override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float MoveRightValue;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float TurnValue;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float LookValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		FCharacterSpeed MovementInfo;
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponDEMO")
		FName InitWeaponName;*/


	void CharacterUpdate();
	void ChangeMovementState();
	UFUNCTION(Server, Reliable)
		void SetMovementState_OnServer(EMovementState NewState);
	UFUNCTION(NetMulticast, Reliable)
		void SetMovementState_Multicast(EMovementState NewState);
	
	void AttackCharEvent(bool bIsFiring);
	void CrouchCharEvent(bool bIsCrouching);
	void JumpCharEvent(bool bIsJumping);

	UFUNCTION()
		void InitWeapon(FName IdWeaponName, FAdditionalWeaponInfo WeaponAdditionalInfo, int32 NewCurrentIndexWeapon);
	void TryReloadWeapon();
	UFUNCTION(Server, Reliable)
		void TryReloadWeapon_OnServer();

	UFUNCTION(Server, Reliable)
		void TrySwitchWeaponToIndexByKeyInput_OnServer(int32 ToIndex);

	UFUNCTION()
		void WeaponFire(UAnimMontage* Anim);
	UFUNCTION()
		void WeaponReloadStart(UAnimMontage* Anim);
	UFUNCTION()
		void WeaponReloadEnd(bool bIsSuccess, int32 AmmoSafe);

	UFUNCTION(BlueprintNativeEvent)
		void WeaponFire_BP(UAnimMontage* Anim);
	UFUNCTION(BlueprintNativeEvent)
		void WeaponReloadStart_BP(UAnimMontage* Anim);
	UFUNCTION(BlueprintNativeEvent)
		void WeaponReloadEnd_BP(bool bIsSuccess);

	UFUNCTION(BlueprintNativeEvent)
		void ToggleADS_BP(bool toggle);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		AWeaponDefault* GetCurrentWeapon();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		int32 GetCurrentWeaponIndex();
	/*UFUNCTION(BlueprintCallable, BlueprintPure)
		EMovementState GetMovementState();*/

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool GetSprintRunEnabled();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool GetCrouchEnabled();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool GetADSEnabled();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool GetIsAlive();
	UFUNCTION(BlueprintNativeEvent)
		void CharacterDead_BP();

	UFUNCTION(NetMulticast, Reliable)
		void PlayAnim_Multicast(UAnimMontage* Anim);

	FTimerHandle TimerHandle_RadDollTimer;

private:

	//Movement flags
	bool SprintRunEnabled = false;
	bool WalkEnabled = false;
	//bool AimEnabled = false;
	bool CrouchEnabled = false;
	bool ADSEnabled = false;

	UPROPERTY(Replicated)
		EMovementState MovementState = EMovementState::Walk_State;

	UPROPERTY(Replicated)
		AWeaponDefault* CurrentWeapon = nullptr;
	

protected:

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	void Turn(float Val);
	void LookUp(float Val);

	void InputSprintPressed();
	void InputSprintReleased();

	void InputWalkPressed();
	void InputWalkReleased();

	void InputJumpPressed();
	void InputJumpReleased();

	void InputAimPressed();
	void InputAimReleased();

	void InputAttackPressed();
	void InputAttackReleased();

	void InputCrouchPressed();
	void InputCrouchReleased();

	void MovementTick();

	void TrySwitchNextWeapon();
	void TrySwitchPreviosWeapon();
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	UFUNCTION()
		void CharacterDead();
	UFUNCTION(NetMulticast, Reliable)
		void EnableRagdoll_Multicast();

	UPROPERTY(Replicated)
		int32 CurrentIndexWeapon = 0;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};

