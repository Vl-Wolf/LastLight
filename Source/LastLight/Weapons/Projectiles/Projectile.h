// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "LastLight/FuncLibrary/Types.h"
#include "Projectile.generated.h"

UCLASS(config=Game)
class LASTLIGHT_API AProjectile : public AActor
{
	GENERATED_BODY()

public:

	AProjectile();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UStaticMeshComponent* BulletMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UProjectileMovementComponent* BulletProjectileMovement = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class USphereComponent* BulletCollisionSphere = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UParticleSystemComponent* BulletFX = nullptr;

	FProjectileInfo ProjectileSetting;

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void InitProjectile(FProjectileInfo InitParam);
	UFUNCTION()
		void BulletCollisionSphereHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
		void ImpactProjectile();
	UFUNCTION()
		void InitVelocity(float InitSpeed, float MaxSpeed);
	UFUNCTION()
		void SpawnHitDecal(UMaterialInterface* DecalMaterial, UPrimitiveComponent* OtherComponent, FHitResult HitResult);
	UFUNCTION()
		void SpawnHitFX(UParticleSystem* FXTemplate, FHitResult HitResult);
	UFUNCTION()
		void SpawnHitSound(USoundBase* HitSound, FHitResult HitResult);
	
};

