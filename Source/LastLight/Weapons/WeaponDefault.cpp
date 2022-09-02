// Fill out your copyright notice in the Description page of Project Settings.


#include "LastLight/Weapons/WeaponDefault.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"

// Sets default values
AWeaponDefault::AWeaponDefault()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = SceneComponent;

	SkeletalMeshWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeleatlMesh"));
	SkeletalMeshWeapon->SetGenerateOverlapEvents(false);
	SkeletalMeshWeapon->SetCollisionProfileName(TEXT("NoCollision"));
	SkeletalMeshWeapon->SetupAttachment(RootComponent);

	StaticMeshWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshWeapon->SetGenerateOverlapEvents(false);
	StaticMeshWeapon->SetCollisionProfileName(TEXT("NoCollision"));
	StaticMeshWeapon->SetupAttachment(RootComponent);

	ShootLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("ShootLocation"));
	ShootLocation->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AWeaponDefault::BeginPlay()
{
	Super::BeginPlay();
	
	WeaponInit();
}

// Called every frame
void AWeaponDefault::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	FireTick(DeltaTime);
	ReloadTick(DeltaTime);
	DispersionTick(DeltaTime);
}

void AWeaponDefault::SetWeaponStateFire(bool bIsFire)
{
	if (CheckWeaponCanFire())
	{
		WeaponFiring = bIsFire;
	}
	else
	{
		WeaponFiring = false;
		FireTimer = 0.01f;
	}
}

bool AWeaponDefault::CheckWeaponCanFire()
{
	return !BlockFire;
}

FProjectileInfo AWeaponDefault::GetProjectile()
{
	return WeaponSetting.ProjectileSetting;
}

FVector AWeaponDefault::GetFireEndLocation() const
{
	bool bShootDirection = false;
	FVector ForwardVector = ShootLocation->GetForwardVector();
	FVector EndLocation = ShootLocation->GetComponentLocation() + ForwardVector * 1000.0f;

	return EndLocation;
}

int8 AWeaponDefault::GetNumberProjectileByShoot() const
{
	return WeaponSetting.NumberProjectileByShot;
}

int32 AWeaponDefault::GetWeaponRound()
{
	return AdditionalWeaponInfo.Round;
}

void AWeaponDefault::InitReload()
{
	WeaponReloading = true;
	ReloadTimer = WeaponSetting.ReloadTime;

	UAnimMontage* AnimPlay = nullptr;
	if (WeaponAiming)
	{
		AnimPlay = WeaponSetting.AnimationWeaponInfo.AnimCharacterReloadAim;
	}
	else
	{
		AnimPlay = WeaponSetting.AnimationWeaponInfo.AnimCharacterReload;
	}

	OnWeaponReloadStart.Broadcast(AnimPlay);

	UAnimMontage* AnimWeaponPlay = nullptr;
	if(WeaponAiming)
	{
		AnimWeaponPlay = WeaponSetting.AnimationWeaponInfo.AnimWeaponReloadAim;
	}
	else
	{
		AnimWeaponPlay = WeaponSetting.AnimationWeaponInfo.AnimWeaponReload;
	}

	if (WeaponSetting.AnimationWeaponInfo.AnimWeaponReload && SkeletalMeshWeapon && SkeletalMeshWeapon->GetAnimInstance())
	{
		AnimWeaponStart(AnimWeaponPlay);
	}
}

void AWeaponDefault::FinishReload()
{
	WeaponReloading = false;
	
	int8 AviableAmmoFromInventory = GetAviableAmmoForReload();
	int8 AmmoNeedTakeFromInventory;
	int8 NeedToReload = WeaponSetting.MaxRound - AdditionalWeaponInfo.Round;

	if (NeedToReload > AviableAmmoFromInventory)
	{
		AdditionalWeaponInfo.Round += AviableAmmoFromInventory;
		AmmoNeedTakeFromInventory = AviableAmmoFromInventory;
	}
	else
	{
		AdditionalWeaponInfo.Round += NeedToReload;
		AmmoNeedTakeFromInventory = NeedToReload;
	}

	OnWeaponReloadEnd.Broadcast(true, -AmmoNeedTakeFromInventory);
}

void AWeaponDefault::CancelReload()
{
	WeaponReloading = false;
	if (SkeletalMeshWeapon && SkeletalMeshWeapon->GetAnimInstance())
	{
		SkeletalMeshWeapon->GetAnimInstance()->StopAllMontages(0.15f);
	}
	
	OnWeaponReloadEnd.Broadcast(false, 0);

}

bool AWeaponDefault::CheckCanWeaponReload()
{
	bool result = true;
	if (GetOwner())
	{
		/////////////////////////////////////////////////////
	}

	return result;
}

int8 AWeaponDefault::GetAviableAmmoForReload()
{
	int8 AviableAmmoForWeapon = WeaponSetting.MaxRound;

	return AviableAmmoForWeapon;
}

void AWeaponDefault::UpdateStateWeapon(EMovementState NewMovementState)
{
	BlockFire = false;

	switch (NewMovementState)
	{
	case EMovementState::Crouch_State:
		WeaponAiming = false;
		CurrentDispersionMax = WeaponSetting.DispersionWeapon.Crouch_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSetting.DispersionWeapon.Crouch_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSetting.DispersionWeapon.Crouch_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSetting.DispersionWeapon.Crouch_StateDispersionAimReduction;
		break;
	case EMovementState::Walk_State:
		WeaponAiming = false;
		CurrentDispersionMax = WeaponSetting.DispersionWeapon.Walk_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSetting.DispersionWeapon.Walk_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSetting.DispersionWeapon.Walk_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSetting.DispersionWeapon.Walk_StateDispersionReduction;
		break;
	case EMovementState::Sprint_State:
		WeaponAiming = false;
		BlockFire = true;
		SetWeaponStateFire(false);
		break;
	default:
		break;
	}
}

void AWeaponDefault::ChangeDispersionByShoot()
{
	CurrentDispersion += CurrentDispersionRecoil;
}

float AWeaponDefault::GetCurrentDispersion() const
{
	float Result = CurrentDispersion;
	return Result;
}

FVector AWeaponDefault::ApplyDispersionToShoot(FVector DirectionShoot) const
{
	return FMath::VRandCone(DirectionShoot, GetCurrentDispersion() * PI/180.0f);
}

void AWeaponDefault::UpdateWeaponCharacterMovementState(FVector NewShootEndLocation, bool NewShouldReduceDispersion)
{
	ShootEndLocation = NewShootEndLocation;
	ShouldReduceDeispersion = NewShouldReduceDispersion;
}

void AWeaponDefault::AnimWeaponStart(UAnimMontage* Anim)
{
	if (Anim && SkeletalMeshWeapon && SkeletalMeshWeapon->GetAnimInstance())
	{
		SkeletalMeshWeapon->GetAnimInstance()->Montage_Play(Anim);
	}
}

void AWeaponDefault::FXWeaponFire(UParticleSystem* FXFire, USoundBase* SoundFire)
{
	if (SoundFire)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), SoundFire, ShootLocation->GetComponentLocation());
	}
	if (FXFire)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FXFire, ShootLocation->GetComponentTransform());
	}
}

void AWeaponDefault::SpawnTraceHitDecal(UMaterialInterface* DecalMaterial, FHitResult HitResult)
{
	UGameplayStatics::SpawnDecalAttached(DecalMaterial, FVector(20.0f), HitResult.GetComponent(), NAME_None, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation(), EAttachLocation::KeepWorldPosition, 10.0f);
}

void AWeaponDefault::SpawnTraceHitSound(USoundBase* HitSound, FHitResult HitResult)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, HitResult.ImpactPoint);
}

void AWeaponDefault::SpawnTraceHitFX(UParticleSystem* FXTemplate, FHitResult HitResult)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld() ,FXTemplate, FTransform(HitResult.ImpactNormal.Rotation(), HitResult.ImpactPoint, FVector(1.0f)));
}

void AWeaponDefault::WeaponInit()
{
	if (SkeletalMeshWeapon && !SkeletalMeshWeapon->SkeletalMesh)
	{
		SkeletalMeshWeapon->DestroyComponent(true);
	}

	if (StaticMeshWeapon && !StaticMeshWeapon->GetStaticMesh())
	{
		StaticMeshWeapon->DestroyComponent(true);
	}

	UpdateStateWeapon(EMovementState::Walk_State);
}

void AWeaponDefault::Fire()
{
	UAnimMontage* AnimPlay = nullptr;
	if (WeaponAiming)
	{
		AnimPlay = WeaponSetting.AnimationWeaponInfo.AnimCharacterFireAim;
	}
	else
	{
		AnimPlay = WeaponSetting.AnimationWeaponInfo.AnimCharacterFire;
	}

	if (WeaponSetting.AnimationWeaponInfo.AnimWeaponFire)
	{
		AnimWeaponStart(WeaponSetting.AnimationWeaponInfo.AnimCharacterFire);
	}

	FireTimer = WeaponSetting.RateOfFire;
	AdditionalWeaponInfo.Round -= 1;
	ChangeDispersionByShoot();

	OnWeaponFire.Broadcast(AnimPlay);

	FXWeaponFire(WeaponSetting.EffectFireWeapon, WeaponSetting.SoundFireWeapon);

	int8 NumberProjectile = GetNumberProjectileByShoot();
	
	if (ShootLocation)
	{
		FVector SpawnLocation = ShootLocation->GetComponentLocation();
		FRotator SpawnRotation = ShootLocation->GetComponentRotation();
		FProjectileInfo ProjectileInfo;
		ProjectileInfo = GetProjectile();
		FVector EndLocation;
		EndLocation = GetFireEndLocation();

		if (ProjectileInfo.Projectile)
		{
			/*FVector Dir = EndLocation - SpawnLocation;
			Dir.Normalize();
			FMatrix myMatrix(Dir, FVector(0, 1, 0), FVector(0, 0, 1), FVector::ZeroVector);
			SpawnRotation = myMatrix.Rotator();*/

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = GetInstigator();

			AProjectile* myProjectile = Cast<AProjectile>(GetWorld()->SpawnActor(ProjectileInfo.Projectile, &SpawnLocation, &SpawnRotation, SpawnParams));
			if (myProjectile)
			{
				myProjectile->InitProjectile(WeaponSetting.ProjectileSetting);
			}

		}
		else
		{
			FHitResult Hit;
			TArray<AActor*> Actors;

			UKismetSystemLibrary::LineTraceSingle(GetWorld(), SpawnLocation, EndLocation,
				ETraceTypeQuery::TraceTypeQuery4, false, Actors, EDrawDebugTrace::ForDuration,
				Hit, true, FLinearColor::Red, FLinearColor::Green, 40.0f);

		}
	}

	if (GetWeaponRound() <= 0 && !WeaponReloading)
	{
		if (CheckCanWeaponReload())
		{
			InitReload();
		}
	}
}

void AWeaponDefault::FireTick(float DeltaTime)
{
	if (WeaponFiring && GetWeaponRound() > 0 && !WeaponReloading)
	{
		if (FireTimer < 0.0f)
		{
			Fire();
		}
		else
		{
			FireTimer -= DeltaTime;
		}
	}
}

void AWeaponDefault::ReloadTick(float DeltaTime)
{
	if (WeaponReloading)
	{
		if (ReloadTimer < 0.0f)
		{
			FinishReload();
		}
		else
		{
			ReloadTimer -= DeltaTime;
		}
	}
}

void AWeaponDefault::DispersionTick(float DeltaTime)
{
	if (!WeaponReloading)
	{
		if (!WeaponFiring)
		{
			if (ShouldReduceDeispersion)
			{
				CurrentDispersion -= CurrentDispersionReduction;
			}
			else
			{
				CurrentDispersion += CurrentDispersionReduction;
			}
		}
		if (CurrentDispersion < CurrentDispersionMin)
		{
			CurrentDispersion = CurrentDispersionMin;
		}
		else
		{
			if (CurrentDispersion > CurrentDispersionMax)
			{
				CurrentDispersion = CurrentDispersionMax;
			}
		}
	}
	if (ShowDebug)
	{
		UE_LOG(LogTemp, Warning, TEXT("Dispersion: MAX = %f. MIN = %f. Current = %f"), CurrentDispersionMax, CurrentDispersionMin, CurrentDispersion);
	}
}

