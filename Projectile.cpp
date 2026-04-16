
#include "Projectile.h"
#include "EnemyBase.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"

AProjectile::AProjectile()
{
 	PrimaryActorTick.bCanEverTick = true;


	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CollisionComponent->InitSphereRadius(5.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
	CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	RootComponent = CollisionComponent;


	ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
	ProjectileMeshComponent->SetupAttachment(RootComponent);
	ProjectileMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere"));
	if (SphereMesh.Succeeded())
	{
		ProjectileMeshComponent->SetStaticMesh(SphereMesh.Object);
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		ProjectileMeshComponent->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
	}


	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->UpdatedComponent = CollisionComponent;
	ProjectileMovementComponent->InitialSpeed = ProjectileSpeed;
	ProjectileMovementComponent->MaxSpeed = ProjectileSpeed;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = false;

	
	TrailEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TrailEffect"));
	TrailEffect->SetupAttachment(RootComponent);
	TrailEffect->bAutoActivate = false;


	InitialLifeSpan = LifeTime;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != GetOwner())
	{
		AEnemyBase* Enemy = Cast<AEnemyBase>(OtherActor);
		if (Enemy)
		{
			UE_LOG(LogTemp, Log, TEXT("Projectile hit enemy for %.0f damage"), Damage);
			Enemy->TakeDamage(Damage);  
		}

		Destroy();
	}
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectile::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileSpeed;
}

