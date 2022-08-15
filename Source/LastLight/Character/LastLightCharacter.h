// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LastLight/FuncLibrary/Types.h"
#include "LastLight/Weapons/WeaponDefault.h"
#include "LastLight/Game/LastLightGameInstance.h"
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

protected:
	virtual void BeginPlay();

public:

	virtual void Tick(float DeltaSeconds) override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		FCharacterSpeed MovementInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponDEMO")
		FName InitWeaponName;


	void CharacterUpdate();
	void ChangeMovementState();
	void SetMovementState(EMovementState NewState);
	
	void AttackCharEvent(bool bIsFiring);

	UFUNCTION(BlueprintCallable)
		//FName WeaponName, FAdditionalWeaponInfo WeaponAdditionalInfo
		void InitWeapon(FName IdWeaponName);
	void TryReloadWeapon();

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

	UFUNCTION(BlueprintCallable, BlueprintPure)
		AWeaponDefault* GetCurrentWeapon();
	UFUNCTION(BlueprintCallable, BlueprintPure)
		EMovementState GetMovementState();


private:

	//Movement flags
	bool SprintRunEnabled = false;
	bool WalkEnabled = false;
	bool AimEnabled = false;

	EMovementState MovementState = EMovementState::Run_State;

	UPROPERTY()
		AWeaponDefault* CurrentWeapon = nullptr;
	

protected:

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	void InputSprintPressed();
	void InputSprintReleased();

	void InputWalkPressed();
	void InputWalkReleased();

	void InputAimPressed();
	void InputAimReleased();

	void InputAttackPressed();
	void InputAttackReleased();

	void MovementTick();
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface
};

