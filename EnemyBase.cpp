#include "EnemyBase.h"
#include "MyArenaCharacter.h"


#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"  
#include "DrawDebugHelpers.h"

 
#include "HealthPickup.h"
#include "AmmoPickup.h"
#include "DamageNumber.h"


AEnemyBase::AEnemyBase()
{
    PrimaryActorTick.bCanEverTick = true;

 
    AIControllerClass = AAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
   


    
    BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
    BodyMesh->SetupAttachment(RootComponent);
    BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube"));
    if (CubeMesh.Succeeded())
    {
        BodyMesh->SetStaticMesh(CubeMesh.Object);
        BodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -50.0f));
        BodyMesh->SetRelativeScale3D(FVector(0.8f, 0.8f, 1.0f));
    }


    static ConstructorHelpers::FObjectFinder<UMaterial> EnemyMaterial(
        TEXT("/Game/Materials/M_Enemy.M_Enemy"));
    if (EnemyMaterial.Succeeded())
    {
        BodyMesh->SetMaterial(0, EnemyMaterial.Object);
    }


    
    GetCapsuleComponent()->SetCapsuleHalfHeight(50.0f);
    GetCapsuleComponent()->SetCapsuleRadius(40.0f);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

    
    GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

    
    bUseControllerRotationYaw = false;
}

void AEnemyBase::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;
    GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

    
    PlayerTarget = Cast<AMyArenaCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    if (PlayerTarget)
    {
        UE_LOG(LogTemp, Log, TEXT("Enemy found player: %s"), *PlayerTarget->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Enemy could not find player!"));
    }

    
    if (BodyMesh)
    {
        UMaterialInterface* Material = BodyMesh->GetMaterial(0);
        if (Material)
        {
            BodyMaterialInstance = UMaterialInstanceDynamic::Create(Material, this);
            if (BodyMaterialInstance)
            {
                BodyMesh->SetMaterial(0, BodyMaterialInstance);

                BodyMaterialInstance->SetVectorParameterValue(FName("Color"), EnemyColor);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("✗ Failed to create dynamic material instance"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("✗ BodyMesh has no material!"));
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Enemy spawned at %s"), *GetActorLocation().ToString());
}

void AEnemyBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsDead)
    {
        return;
    }

    
    if (PlayerTarget)
    {
        AMyArenaCharacter* Character = Cast<AMyArenaCharacter>(PlayerTarget);
        if (Character && Character->IsDead())
        {
            return;
        }
    }

    DrawDebugLine(
        GetWorld(),
        GetActorLocation(),
        PlayerTarget->GetActorLocation(),
        FColor::Green,
        false,
        0.1f,
        0,
        5.0f
    );


    MoveTowardsPlayer(DeltaTime);
    TryAttackPlayer();
}

void AEnemyBase::TryDropPickup()
{
    float HealthRoll = FMath::FRand();
    float AmmoRoll = FMath::FRand();

    FVector DropLocation = GetActorLocation();

    
    if (HealthRoll <= HealthDropChance)
    {
        AHealthPickup* HealthPickup = GetWorld()->SpawnActor<AHealthPickup>(
            AHealthPickup::StaticClass(),  
            DropLocation,
            FRotator::ZeroRotator
        );

        if (HealthPickup)
        {
            UE_LOG(LogTemp, Warning, TEXT("✓ Health pickup spawned!"));
        }
        return;
    }

    if (AmmoRoll <= AmmoDropChance)
    {
        
        AAmmoPickup* AmmoPickup = GetWorld()->SpawnActor<AAmmoPickup>(
            AAmmoPickup::StaticClass(),  
            DropLocation,
            FRotator::ZeroRotator
        );

        if (AmmoPickup)
        {
            UE_LOG(LogTemp, Warning, TEXT("✓ Ammo pickup spawned!"));
        }
    }
}

void AEnemyBase::SpawnDamageNumber(float DamageAmount)
{

    UE_LOG(LogTemp, Warning, TEXT("SpawnDamageNumber called: %.0f"), DamageAmount);
    UE_LOG(LogTemp, Error, TEXT("Enemy class spawning: %s"),
        *GetClass()->GetName());


    if (!DamageNumberClass)
    {
        UE_LOG(LogTemp, Error, TEXT("✗ DamageNumberClass is NULL"));
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("✓ DamageNumberClass valid: %s"), *DamageNumberClass->GetName());

    FVector SpawnLocation = GetActorLocation() + FVector(
        FMath::FRandRange(-25.0f, 25.0f),
        FMath::FRandRange(-25.0f, 25.0f),
        80.0f
    );

    ADamageNumber* DmgNumber = GetWorld()->SpawnActor<ADamageNumber>(
        DamageNumberClass,
        SpawnLocation,
        FRotator::ZeroRotator
    );

    if (!DmgNumber)
    {
        UE_LOG(LogTemp, Error, TEXT("✗ SpawnActor returned NULL for DamageNumber"));
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("✓ DamageNumber actor spawned at %s"), *SpawnLocation.ToString());

    DmgNumber->SetDamageText(DamageAmount);
}

void AEnemyBase::MoveTowardsPlayer(float DeltaTime)
{
    if (!PlayerTarget)
    {
        return;
    }

    float DistanceToPlayer = FVector::Dist(GetActorLocation(), PlayerTarget->GetActorLocation());

    
    if (DistanceToPlayer > ChaseRange)
    {
        return;
    }

    
    if (DistanceToPlayer <= AttackRange)
    {
        return;
    }

    
    FVector Direction = (PlayerTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    AddMovementInput(Direction, 1.0f);
}

void AEnemyBase::TryAttackPlayer()
{
    if (!PlayerTarget || bIsDead)
    {
        return;
    }

    float DistanceToPlayer = FVector::Dist(GetActorLocation(), PlayerTarget->GetActorLocation());

   
    if (DistanceToPlayer > AttackRange)
    {
        return;
    }

   
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastAttackTime < AttackCooldown)
    {
        return;
    }

    LastAttackTime = CurrentTime;

    
    PlayerTarget->TakeDamage(AttackDamage);

    UE_LOG(LogTemp, Warning, TEXT("Enemy attacked player for %.0f damage!"), AttackDamage);

   
}

void AEnemyBase::TakeDamage(float DamageAmount)
{
    if (bIsDead)
    {
        return;
    }

    CurrentHealth -= DamageAmount;
    CurrentHealth = FMath::Max(0.0f, CurrentHealth);

    SpawnDamageNumber(DamageAmount);

    UE_LOG(LogTemp, Log, TEXT("Enemy took %.0f damage! Health: %.0f/%.0f"),
        DamageAmount, CurrentHealth, MaxHealth);

  
    FlashDamageColor();

  
    if (HitSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
    }

   
    if (CurrentHealth <= 0.0f)
    {
        Die();
    }
}

void AEnemyBase::FlashDamageColor()
{
    if (BodyMaterialInstance)
    {
       
        BodyMaterialInstance->SetVectorParameterValue(FName("Color"), FLinearColor::White);
        UE_LOG(LogTemp, Log, TEXT("Enemy flashing white"));

       
        FTimerHandle ResetColorTimer;
        GetWorld()->GetTimerManager().SetTimer(ResetColorTimer, [this]()
            {
                if (BodyMaterialInstance && !bIsDead)
                {
                    BodyMaterialInstance->SetVectorParameterValue(FName("Color"), EnemyColor);
                    UE_LOG(LogTemp, Log, TEXT("Enemy color reset to red"));
                }
            }, 0.15f, false);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("BodyMaterialInstance is NULL - can't flash color"));

    
        if (BodyMesh)
        {
            FVector OriginalScale = BodyMesh->GetRelativeScale3D();
            BodyMesh->SetRelativeScale3D(OriginalScale * 1.2f);

            FTimerHandle ScaleTimer;
            GetWorld()->GetTimerManager().SetTimer(ScaleTimer, [this, OriginalScale]()
                {
                    if (BodyMesh)
                    {
                        BodyMesh->SetRelativeScale3D(OriginalScale);
                    }
                }, 0.15f, false);
        }
    }
}

void AEnemyBase::Die()
{
    if (bIsDead)
    {
        return;
    }

    bIsDead = true;

    UE_LOG(LogTemp, Warning, TEXT("Enemy died!"));


    if (DeathSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
    }

  
    
    AMyArenaCharacter* Player = Cast<AMyArenaCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (Player)
    {
        Player->AddScore(ScoreValue);
    }

    TryDropPickup();

  
    if (DeathEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathEffect, GetActorLocation());
    }

 
    if (DeathSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
    }

   
    SetLifeSpan(0.1f);
}