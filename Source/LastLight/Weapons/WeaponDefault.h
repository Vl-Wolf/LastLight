// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LastLight/FuncLibrary/Types.h"
#include "Components/ArrowComponent.h"
#include "LastLight/Weapons/Projectiles/Projectile.h"
#include "WeaponDefault.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponReloadStart, UAnimMontage*, AnimReloadStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponReloadEnd, bool, bIsSuccess, int32, AmmoSafe);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponFire, UAnimMontage*, AnimFireChar);

UCLASS()
class LASTLIGHT_API AWeaponDefault : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponDefault();

	FOnWeaponReloadStart OnWeaponReloadStart;
	FOnWeaponReloadEnd OnWeaponReloadEnd;
	FOnWeaponFire OnWeaponFire;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class USceneComponent* SceneComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class USkeletalMeshComponent* SkeletalMeshWeapon = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UStaticMeshComponent* StaticMeshWeapon = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UArrowComponent* ShootLocation = nullptr;

	UPROPERTY(VisibleAnywhere)
		FWeaponInfo WeaponSetting;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
		FAdditionalWeaponInfo AdditionalWeaponInfo;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configurations")
		FTransform PlacementTransform;*/

		float FireTimer = 0.0f;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Reload Logic")
		float ReloadTimer = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireLogic")
		FName IdWeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire Logic")
		bool WeaponFiring = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reload  Logic")
		bool WeaponReloading = false;
	bool WeaponAiming = false;
	bool BlockFire = false;

	FVector ShootEndLocation = FVector(0);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void WeaponInit();
	
	void FireTick(float DeltaTime);
	void ReloadTick(float DeltaTime);
	void DispersionTick(float DeltaTime);

	UFUNCTION()
		void Fire();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void SetWeaponStateFire(bool bIsFire);

	bool CheckWeaponCanFire();

	FProjectileInfo GetProjectile();
	FVector GetFireEndLocation() const;
	int8 GetNumberProjectileByShoot() const;

	UFUNCTION(BlueprintCallable)
		int32 GetWeaponRound();

	UFUNCTION()
		void InitReload();
	void FinishReload();
	void CancelReload();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		bool ShowDebug = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		float SizeVectorToChangeShootDirectionLogic = 100.0f;

	bool CheckCanWeaponReload();
	int8 GetAviableAmmoForReload();

	void UpdateStateWeapon(EMovementState NewMovementState);
	void ChangeDispersionByShoot();
	float GetCurrentDispersion() const;
	FVector ApplyDispersionToShoot(FVector DirectionShoot) const;

	void UpdateWeaponCharacterMovementState(FVector NewShootEndLocation, bool NewShouldReduceDispersion);

	//Dispersion
	bool ShouldReduceDeispersion = false;
	float CurrentDispersion = 0.0f;
	float CurrentDispersionMax = 1.0f;
	float CurrentDispersionMin = 0.1f;
	float CurrentDispersionRecoil = 0.1f;
	float CurrentDispersionReduction = 0.1f;

	UFUNCTION()
		void AnimWeaponStart(UAnimMontage* Anim);
	UFUNCTION()
		void FXWeaponFire(UParticleSystem* FXFire, USoundBase* SoundFire);
	UFUNCTION()
		void SpawnTraceHitDecal(UMaterialInterface* DecalMaterial, FHitResult HitResult);
	UFUNCTION()
		void SpawnTraceHitFX(UParticleSystem* FXTemplate, FHitResult HitResult);
	UFUNCTION()
		void SpawnTraceHitSound(USoundBase* HitSound, FHitResult HitResult);

};
