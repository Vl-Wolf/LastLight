// Copyright Epic Games, Inc. All Rights Reserved.

#include "LastLightCharacter.h"
#include "LastLight/Weapons/Projectiles/Projectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ALastLightCharacter

ALastLightCharacter::ALastLightCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	PrimaryActorTick.bCanEverTick = false;



	// Create a CameraComponent	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	Camera->SetupAttachment(GetMesh(), FName("head"));
	Camera->bUsePawnControlRotation = true;
}

void ALastLightCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	InitWeapon(InitWeaponName);
}

void ALastLightCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	MovementTick();
}


//////////////////////////////////////////////////////////////////////////
// Input
void ALastLightCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ALastLightCharacter::InputAttackPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ALastLightCharacter::InputAttackReleased);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ALastLightCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALastLightCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);


	PlayerInputComponent->BindAction("ChangeToSprint", EInputEvent::IE_Pressed, this, &ALastLightCharacter::InputSprintPressed);
	PlayerInputComponent->BindAction("ChangeToSprint", EInputEvent::IE_Released, this, &ALastLightCharacter::InputSprintReleased);

	PlayerInputComponent->BindAction("ChangeToWalk", EInputEvent::IE_Pressed, this, &ALastLightCharacter::InputWalkPressed);
	PlayerInputComponent->BindAction("ChangeToWalk", EInputEvent::IE_Released, this, &ALastLightCharacter::InputWalkReleased);

	PlayerInputComponent->BindAction("AimEvent", EInputEvent::IE_Pressed, this, &ALastLightCharacter::InputAimPressed);
	PlayerInputComponent->BindAction("AimEvent", EInputEvent::IE_Released, this, &ALastLightCharacter::InputAimReleased);
}

void ALastLightCharacter::InputSprintPressed()
{
	SprintRunEnabled = true;
	ChangeMovementState();
}

void ALastLightCharacter::InputSprintReleased()
{
	SprintRunEnabled = false;
	ChangeMovementState();
}

void ALastLightCharacter::InputWalkPressed()
{
	WalkEnabled = true;
	ChangeMovementState();
}

void ALastLightCharacter::InputWalkReleased()
{
	WalkEnabled = false;
	ChangeMovementState();
}

void ALastLightCharacter::InputAimPressed()
{
	AimEnabled = true;
	ChangeMovementState();
}

void ALastLightCharacter::InputAimReleased()
{
	AimEnabled = false;
	ChangeMovementState();
}

void ALastLightCharacter::InputAttackPressed()
{
	AttackCharEvent(true);
}

void ALastLightCharacter::InputAttackReleased()
{
	AttackCharEvent(false);
}


void ALastLightCharacter::CharacterUpdate()
{
	float ResSpeed = 600.0f;
	switch (MovementState)
	{
	case EMovementState::Aim_State:
		ResSpeed = MovementInfo.AimSpeed;
		break;
	case EMovementState::Walk_State:
		ResSpeed = MovementInfo.WalkSpeed;
		break;
	case EMovementState::Run_State:
		ResSpeed = MovementInfo.RunSpeed;
		break;
	case EMovementState::AimWalk_State:
		ResSpeed = MovementInfo.AimWalkSpeed;
		break;
	case EMovementState::Sprint_State:
		ResSpeed = MovementInfo.SprintSpeed;
		break;
	default:
		break;
	}

	GetCharacterMovement()->MaxWalkSpeed = ResSpeed;
}

void ALastLightCharacter::ChangeMovementState()
{
	EMovementState NewState = EMovementState::Run_State;

	if (!SprintRunEnabled && !WalkEnabled && !AimEnabled)
	{
		NewState = EMovementState::Run_State;
	}
	else
	{
		if (SprintRunEnabled)
		{
			NewState = EMovementState::Sprint_State;
			WalkEnabled = false;
			AimEnabled = false;
		}
		else
		{
			if (!SprintRunEnabled && WalkEnabled && AimEnabled)
			{
				NewState = EMovementState::AimWalk_State;
			}
			else
			{
				if (!SprintRunEnabled && WalkEnabled && !AimEnabled)
				{
					NewState = EMovementState::Walk_State;
				}
				else
				{
					if (!SprintRunEnabled && !WalkEnabled && AimEnabled)
					{
						NewState = EMovementState::Aim_State;
					}
				}
			}
		}
	}

	SetMovementState(NewState);

	AWeaponDefault* myWeapon = GetCurrentWeapon();
	if (myWeapon)
	{
		myWeapon->UpdateStateWeapon(NewState);
	}
}

void ALastLightCharacter::SetMovementState(EMovementState NewState)
{
	MovementState = NewState;
	CharacterUpdate();
}

AWeaponDefault* ALastLightCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}

EMovementState ALastLightCharacter::GetMovementState()
{
	return MovementState;
}

void ALastLightCharacter::MovementTick()
{
	FHitResult HitResult;

	if (CurrentWeapon)
	{
		FVector Displacement = FVector(0);
		bool bIsReduceDispersion = false;
		switch (MovementState)
		{
		case EMovementState::Aim_State:
			Displacement = FVector(0.0f, 0.0f, 160.0f);
			bIsReduceDispersion = true;
			break;
		case EMovementState::AimWalk_State:
			Displacement = FVector(0.0f, 0.0f, 160.0f);
			bIsReduceDispersion = true;
			break;
		case EMovementState::Walk_State:
			Displacement = FVector(0.0f, 0.0f, 120.0f);
			break;
		case EMovementState::Run_State:
			Displacement = FVector(0.0f, 0.0f, 120.0f);
			break;
		case EMovementState::Sprint_State:
			break;
		default:
			break;
		}

		CurrentWeapon->UpdateWeaponCharacterMovementState(HitResult.Location + Displacement, bIsReduceDispersion);
	}
}

void ALastLightCharacter::AttackCharEvent(bool bIsFiring)
{
	AWeaponDefault* myWeapon = nullptr;
	myWeapon = GetCurrentWeapon();
	if (myWeapon)
	{
		myWeapon->SetWeaponStateFire(bIsFiring);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ALastLight::AttackCharEvent - CurrentWeapon -NULL"));
	}
}

void ALastLightCharacter::InitWeapon(FName IdWeaponName)
{//FName WeaponName, FAdditionalWeaponInfo WeaponAdditionalInfo
	ULastLightGameInstance* myGI = Cast<ULastLightGameInstance>(GetGameInstance());
	FWeaponInfo myWeaponInfo;

	if (myGI)
	{
		if (myGI->GetWeaponInfoByName(IdWeaponName, myWeaponInfo))
		{
			if (myWeaponInfo.WeaponClass)
			{
				FVector SpawnLocation = FVector(0);
				FRotator SpawnRotation = FRotator(0);
				FActorSpawnParameters SpawnParams;

				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = GetInstigator();

				AWeaponDefault* myWeapon = Cast<AWeaponDefault>(GetWorld()->SpawnActor(myWeaponInfo.WeaponClass, &SpawnLocation, &SpawnRotation, SpawnParams));
				if (myWeapon)
				{
					const FTransform& PlacementTransform = myWeapon->PlacementTransform * GetMesh()->GetSocketTransform(FName("WeaponSocketRightHand"));
					myWeapon->SetActorTransform(PlacementTransform, false, nullptr, ETeleportType::TeleportPhysics);
					myWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, FName("WeaponSocketRightHand"));
					CurrentWeapon = myWeapon;

					//myWeapon->IdWeaponName = WeaponName;
					
					myWeapon->WeaponSetting = myWeaponInfo;
					myWeapon->ReloadTimer = myWeaponInfo.ReloadTime;
					myWeapon->AdditionalWeaponInfo.Round = myWeaponInfo.MaxRound;
					
					
					myWeapon->OnWeaponReloadStart.AddDynamic(this, &ALastLightCharacter::WeaponReloadStart);
					myWeapon->OnWeaponReloadEnd.AddDynamic(this, &ALastLightCharacter::WeaponReloadEnd);
					myWeapon->OnWeaponFire.AddDynamic(this, &ALastLightCharacter::WeaponFire);

					if (CurrentWeapon->GetWeaponRound() <= 0 && CurrentWeapon->CheckCanWeaponReload())
					{
						CurrentWeapon->InitReload();
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("ALastLight::InitWeapon - WeaponClass - NULL"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ALastLight::InitWeapon - Weapon not found in table - NULL"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ALastLight::InitWeapon - GI - NULL"));
	}
}

void ALastLightCharacter::TryReloadWeapon()
{
	if (CurrentWeapon && !CurrentWeapon->WeaponAiming)
	{
		if (CurrentWeapon->GetWeaponRound() < CurrentWeapon->WeaponSetting.MaxRound && CurrentWeapon->CheckCanWeaponReload())
		{
			CurrentWeapon->InitReload();
		}
	}
}

void ALastLightCharacter::WeaponFire(UAnimMontage* Anim)
{
	if(CurrentWeapon)
	{
		WeaponFire_BP(Anim);
	}
}

void ALastLightCharacter::WeaponReloadStart(UAnimMontage* Anim)
{
	WeaponReloadStart_BP(Anim);
}

void ALastLightCharacter::WeaponReloadEnd(bool bIsSuccess, int32 AmmoSafe)
{
	WeaponReloadEnd_BP(bIsSuccess);
}

void ALastLightCharacter::WeaponFire_BP_Implementation(UAnimMontage* Anim)
{
	//In Blueprint
}

void ALastLightCharacter::WeaponReloadStart_BP_Implementation(UAnimMontage* Anim)
{
	//In Blueprint
}

void ALastLightCharacter::WeaponReloadEnd_BP_Implementation(bool bIsSuccess)
{
	//In Blueprint
}

void ALastLightCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ALastLightCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}