
#include "BaseWeapon.h"
#include "Projectile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameHUD.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraShakeBase.h"
#include "MyArenaCharacter.h"

ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;


    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    
    WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
    WeaponMesh->SetupAttachment(RootComponent);
    WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    
    MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
    MuzzleLocation->SetupAttachment(WeaponMesh);
    MuzzleLocation->SetRelativeLocation(FVector(50.0f, 0.0f, 0.0f));


    static ConstructorHelpers::FClassFinder<AProjectile> ProjectileBP(TEXT("/Game/Weapon/Blueprint/BP_Projectile"));
    if (ProjectileBP.Succeeded())
    {
        ProjectileClass = ProjectileBP.Class;
    }
}

void ABaseWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ABaseWeapon::Fire()
{
    

    if (!CanFire())
    {
        
        return;
    }

    if (CurrentAmmo <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("No ammo!"));
        return;
    }

    CurrentAmmo--;

    UE_LOG(LogTemp, Warning, TEXT("%s fired! Damage: %.0f | Ammo: %d/%d"),
        *WeaponName, Damage, CurrentAmmo, MaxAmmo);
   
    AActor* WeaponOwner = GetOwner();
    if (WeaponOwner)
    {
        AMyArenaCharacter* Character = Cast<AMyArenaCharacter>(WeaponOwner);
        if (Character && Character->GetGameHUD())
        {
            Character->GetGameHUD()->UpdateAmmo(WeaponName, CurrentAmmo, MaxAmmo);
        }
    }

    if (FireSounds.Num() > 0)
    {
        
        int32 RandomIndex = FMath::RandRange(0, FireSounds.Num() - 1);
        USoundBase* SelectedSound = FireSounds[RandomIndex];

        if (SelectedSound)
        {
            
            float PitchMultiplier = FMath::FRandRange(0.95f, 1.05f);

            UGameplayStatics::PlaySoundAtLocation(
                this,
                SelectedSound,
                GetActorLocation(),
                1.0f, 
                PitchMultiplier  
            );

            UE_LOG(LogTemp, Log, TEXT("Playing fire sound %d (pitch: %.2f)"), RandomIndex, PitchMultiplier);
        }
    }

    if (FireCameraShake)
    {
        WeaponOwner = GetOwner();
        if (WeaponOwner)
        {
            APlayerController* PC = Cast<APlayerController>(WeaponOwner->GetInstigatorController());
            if (PC && PC->PlayerCameraManager)
            {
                PC->PlayerCameraManager->StartCameraShake(FireCameraShake, 1.0f);
            }
        }
    }

    for (int32 i = 0; i < ProjectilesPerShot; i++)
    {
        if (ProjectileClass && MuzzleLocation)
        {
            FVector SpawnLocation = MuzzleLocation->GetComponentLocation();
            FRotator SpawnRotation = MuzzleLocation->GetComponentRotation();

            UE_LOG(LogTemp, Log, TEXT("Spawning projectile %d at %s"), i, *SpawnLocation.ToString());


            
            float RandomSpreadYaw = FMath::FRandRange(-SpreadAngle, SpreadAngle);
            float RandomSpreadPitch = FMath::FRandRange(-SpreadAngle, SpreadAngle);

            SpawnRotation.Yaw += RandomSpreadYaw;
            SpawnRotation.Pitch += RandomSpreadPitch;

            AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(
                ProjectileClass,
                SpawnLocation,
                SpawnRotation
            );

            if (Projectile)
            {
                UE_LOG(LogTemp, Warning, TEXT("✓ Projectile %d spawned!"), i);
                Projectile->SetOwner(Owner);
                Projectile->SetDamage(Damage);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("✗ Failed to spawn projectile %d!"), i);
            }

        }
        else
        {
            if (!ProjectileClass)
            {
                UE_LOG(LogTemp, Error, TEXT("✗ ProjectileClass is NULL!"));
            }
            if (!MuzzleLocation)
            {
                UE_LOG(LogTemp, Error, TEXT("✗ MuzzleLocation is NULL!"));
            }
        }
    }


    

    
    bCanFire = false;
    GetWorld()->GetTimerManager().SetTimer(FireRateTimerHandle, [this]()
        {
            bCanFire = true;
        }, FireRate, false);





    if (WeaponMesh)
    {
        FVector OriginalLoc = WeaponMesh->GetRelativeLocation();
        WeaponMesh->SetRelativeLocation(OriginalLoc + FVector(-10.0f, 0.0f, 0.0f));

        
        FTimerHandle RecoilTimer;
        GetWorld()->GetTimerManager().SetTimer(RecoilTimer, [this, OriginalLoc]()
            {
                if (WeaponMesh)
                {
                    WeaponMesh->SetRelativeLocation(OriginalLoc);
                }
            }, 0.1f, false);
    }

    
    if (MuzzleFlashEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            MuzzleFlashEffect,
            MuzzleLocation->GetComponentLocation(),  
            GetActorRotation()
        );
    }
}

bool ABaseWeapon::CanFire() const
{
    bool bHasAmmo = CurrentAmmo > 0;
    bool bCooldownReady = bCanFire;

    if (!bHasAmmo)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s: No ammo (%d/%d)"), *WeaponName, CurrentAmmo, MaxAmmo);
    }
    if (!bCooldownReady)
    {
        
    }

    return bHasAmmo && bCooldownReady;
}


void ABaseWeapon::AddAmmo(int32 Amount)
{
    int32 OldAmmo = CurrentAmmo;
    CurrentAmmo += Amount;

    
    if (CurrentAmmo > MaxAmmo)
    {
        CurrentAmmo = MaxAmmo;
    }

    int32 ActualAdded = CurrentAmmo - OldAmmo;

    UE_LOG(LogTemp, Log, TEXT("%s: Added %d ammo (%d -> %d)"),
        *WeaponName, ActualAdded, OldAmmo, CurrentAmmo);

    
    AActor* WeaponOwner = GetOwner();
    if (WeaponOwner)
    {
        AMyArenaCharacter* Character = Cast<AMyArenaCharacter>(WeaponOwner);
        if (Character && Character->GetGameHUD())
        {
            Character->GetGameHUD()->UpdateAmmo(WeaponName, CurrentAmmo, MaxAmmo);
        }
    }
}


void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

    CurrentAmmo = MaxAmmo;
    bCanFire = true;

    UE_LOG(LogTemp, Log, TEXT("%s initialized with %d/%d ammo"), *WeaponName, CurrentAmmo, MaxAmmo);
}

void ABaseWeapon::SpawnProjectile(const FVector& Direction)
{
    if (!ProjectileClass)
    {
        UE_LOG(LogTemp, Error, TEXT("ProjectileClass is NULL!"));
        return;
    }

    UWorld* World = GetWorld();
    if (!World) return;

    FVector SpawnLocation = MuzzleLocation->GetComponentLocation();
    FRotator SpawnRotation = Direction.Rotation();

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = GetOwner();
    SpawnParams.Instigator = Cast<APawn>(GetOwner());

    AProjectile* Projectile = World->SpawnActor<AProjectile>(
        ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

    if (Projectile)
    {
        Projectile->Damage = Damage;
        Projectile->FireInDirection(Direction);
    }
}



