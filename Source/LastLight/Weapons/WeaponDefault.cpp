// Fill out your copyright notice in the Description page of Project Settings.


#include "LastLight/Weapons/WeaponDefault.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "LastLight/Character/LastLightInventoryComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AWeaponDefault::AWeaponDefault()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);

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

	if(HasAuthority())
	{
		FireTick(DeltaTime);
		ReloadTick(DeltaTime);
		DispersionTick(DeltaTime);
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

	UpdateStateWeapon_OnServer(EMovementState::Walk_State);
}

void AWeaponDefault::SetWeaponStateFire_OnServer_Implementation(bool bIsFire)
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
		AnimWeaponStart_Multicast(WeaponSetting.AnimationWeaponInfo.AnimCharacterFire);
	}

	FireTimer = WeaponSetting.RateOfFire;
	AdditionalWeaponInfo.Round -= 1;
	ChangeDispersionByShoot();

	OnWeaponFire.Broadcast(AnimPlay);

	FXWeaponFire_Multicast(WeaponSetting.EffectFireWeapon, WeaponSetting.SoundFireWeapon);
	
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

void AWeaponDefault::UpdateStateWeapon_OnServer_Implementation(EMovementState NewMovementState)
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
		SetWeaponStateFire_OnServer(false);
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
		AnimWeaponStart_Multicast(AnimWeaponPlay);
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
		ULastLightInventoryComponent* myInv = Cast<ULastLightInventoryComponent>(GetOwner()->GetComponentByClass(ULastLightInventoryComponent::StaticClass()));
		if (myInv)
		{
			int8 AviableAmmoForWeapon;
			if (!myInv->CheckAmmoForWeapon(WeaponSetting.WeaponType, AviableAmmoForWeapon))
			{
				result = false;
				myInv->OnWeaponNotHaveRound.Broadcast(myInv->GetWeaponIndexSlotByName(IdWeaponName));
			}
			else
			{
				myInv->OnWeaponHaveRound.Broadcast(myInv->GetWeaponIndexSlotByName(IdWeaponName));
			}
		}
	}

	return result;
}

int8 AWeaponDefault::GetAviableAmmoForReload()
{
	int8 AviableAmmoForWeapon = WeaponSetting.MaxRound;

	if(GetOwner())
	{
		ULastLightInventoryComponent* myInv = Cast<ULastLightInventoryComponent>(GetOwner()->GetComponentByClass(ULastLightInventoryComponent::StaticClass()));
		if (myInv)
		{
			if (myInv->CheckAmmoForWeapon(WeaponSetting.WeaponType, AviableAmmoForWeapon))
			{
				AviableAmmoForWeapon = AviableAmmoForWeapon;
			}
		}
	}

	return AviableAmmoForWeapon;
}

void AWeaponDefault::FXWeaponFire_Multicast_Implementation(UParticleSystem* FXFire, USoundBase* SoundFire)
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

void AWeaponDefault::AnimWeaponStart_Multicast_Implementation(UAnimMontage* Anim)
{
	if (Anim && SkeletalMeshWeapon && SkeletalMeshWeapon->GetAnimInstance())
	{
		SkeletalMeshWeapon->GetAnimInstance()->Montage_Play(Anim);
	}
}

void AWeaponDefault::UpdateWeaponCharacterMovementState_OnServer_Implementation(FVector NewShootEndLocation,
	bool NewShouldReduceDispersion)
{
	ShootEndLocation = NewShootEndLocation;
	ShouldReduceDeispersion = NewShouldReduceDispersion;
}

void AWeaponDefault::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponDefault, AdditionalWeaponInfo);
	DOREPLIFETIME(AWeaponDefault, WeaponReloading);
	DOREPLIFETIME(AWeaponDefault, ShootEndLocation);

}

void AWeaponDefault::SpawnTraceHitSound_Multicast_Implementation(USoundBase* HitSound, FHitResult HitResult)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, HitResult.ImpactPoint);
}

void AWeaponDefault::SpawnTraceHitFX_Multicast_Implementation(UParticleSystem* FXTemplate, FHitResult HitResult)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld() ,FXTemplate, FTransform(HitResult.ImpactNormal.Rotation(), HitResult.ImpactPoint, FVector(1.0f)));
}

void AWeaponDefault::SpawnTraceHitDecal_Multicast_Implementation(UMaterialInterface* DecalMaterial,
	FHitResult HitResult)
{
	UGameplayStatics::SpawnDecalAttached(DecalMaterial, FVector(20.0f), HitResult.GetComponent(), NAME_None,
		HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation(), EAttachLocation::KeepWorldPosition, 10.0f);
}