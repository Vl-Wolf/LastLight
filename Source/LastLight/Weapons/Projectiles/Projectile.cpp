#include "Projectile.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"

AProjectile::AProjectile() 
{

	PrimaryActorTick.bCanEverTick = true;

	BulletCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
	BulletCollisionSphere->SetSphereRadius(16.0f);
	BulletCollisionSphere->bReturnMaterialOnMove = true;
	BulletCollisionSphere->SetCanEverAffectNavigation(false);

	RootComponent = BulletCollisionSphere;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bullet Projectile Mesh"));
	BulletMesh->SetupAttachment(RootComponent);
	BulletMesh->SetCanEverAffectNavigation(false);

	BulletFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Bullet FX"));
	BulletFX->SetupAttachment(RootComponent);

	BulletProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Bullet ProjectileMovement"));
	BulletProjectileMovement->UpdatedComponent = RootComponent;
	BulletProjectileMovement->bRotationFollowsVelocity = true;
	BulletProjectileMovement->bShouldBounce = true;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	BulletCollisionSphere->OnComponentHit.AddDynamic(this, &AProjectile::BulletCollisionSphereHit);
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectile::InitProjectile(FProjectileInfo InitParam)
{
	this->SetLifeSpan(InitParam.ProjectileLifeTime);

	if (InitParam.ProjectileStaticMesh)
	{
		BulletMesh->SetStaticMesh(InitParam.ProjectileStaticMesh);
		BulletMesh->SetRelativeTransform(InitParam.ProjectileStaticMeshOffset);
	}
	else
	{
		BulletMesh->DestroyComponent();
	}

	if (InitParam.ProjectileTrailFX)
	{
		BulletFX->SetTemplate(InitParam.ProjectileTrailFX);
		BulletFX->SetRelativeTransform(InitParam.ProjectileTrailFXOffset);
	}
	else
	{
		BulletFX->DestroyComponent();
	}

	InitVelocity(InitParam.ProjectileInitSpeed, InitParam.ProjectileMaxSpeed);

	ProjectileSetting = InitParam;
}

void AProjectile::BulletCollisionSphereHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && Hit.PhysMaterial.IsValid())
	{
		EPhysicalSurface mySurfaceType = UGameplayStatics::GetSurfaceType(Hit);

		if (ProjectileSetting.HitDecals.Contains(mySurfaceType))
		{
			UMaterialInterface* myMaterial = ProjectileSetting.HitDecals[mySurfaceType];

			if (myMaterial && OtherComp)
			{
				SpawnHitDecal(myMaterial, OtherComp, Hit);
			}
		}

		if (ProjectileSetting.HitFXs.Contains(mySurfaceType))
		{
			UParticleSystem* myParticle = ProjectileSetting.HitFXs[mySurfaceType];
			{
				if (myParticle)
				{
					SpawnHitFX(myParticle, Hit);
				}
			}
		}
		
		if (ProjectileSetting.HitSound)
		{
			SpawnHitSound(ProjectileSetting.HitSound, Hit);
		}
	}

	UGameplayStatics::ApplyPointDamage(OtherActor, ProjectileSetting.ProjectileDamage, Hit.TraceStart, Hit, GetInstigatorController(), this, NULL);
	ImpactProjectile();
}

void AProjectile::ImpactProjectile()
{
	this->Destroy();
}

void AProjectile::InitVelocity(float InitSpeed, float MaxSpeed)
{
	if (BulletProjectileMovement)
	{
		BulletProjectileMovement->Velocity = GetActorForwardVector() * InitSpeed;
		BulletProjectileMovement->MaxSpeed = MaxSpeed;
		BulletProjectileMovement->InitialSpeed = InitSpeed;
	}
}

void AProjectile::SpawnHitDecal(UMaterialInterface* DecalMaterial, UPrimitiveComponent* OtherComponent, FHitResult HitResult)
{
	UGameplayStatics::SpawnDecalAttached(DecalMaterial, FVector(20.0f), OtherComponent, NAME_None, HitResult.ImpactPoint ,HitResult.ImpactNormal.Rotation(), EAttachLocation::KeepWorldPosition, 10.0f);
}

void AProjectile::SpawnHitFX(UParticleSystem* FXTemplate, FHitResult HitResult)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FXTemplate, FTransform(HitResult.ImpactNormal.Rotation(), HitResult.ImpactPoint, FVector(1.0f)));
}

void AProjectile::SpawnHitSound(USoundBase* HitSound, FHitResult HitResult)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, HitResult.ImpactPoint);
}
