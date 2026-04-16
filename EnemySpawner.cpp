#include "EnemySpawner.h"
#include "EnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "WaveManager.h"

AEnemySpawner::AEnemySpawner()
{
    PrimaryActorTick.bCanEverTick = true;

    
    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;
}

void AEnemySpawner::BeginPlay()
{
    Super::BeginPlay();


    UE_LOG(LogTemp, Warning, TEXT("=== EnemySpawner BeginPlay ==="));
    UE_LOG(LogTemp, Warning, TEXT("Location: %s"), *GetActorLocation().ToString());
    UE_LOG(LogTemp, Warning, TEXT("Auto Start: %s"), bAutoStart ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("Enemy Class: %s"), EnemyClass ? *EnemyClass->GetName() : TEXT("NULL"));
    UE_LOG(LogTemp, Warning, TEXT("Spawn Interval: %.1f"), SpawnInterval);
    UE_LOG(LogTemp, Warning, TEXT("Max Enemies: %d"), MaxEnemies);


    if (bAutoStart)
    {
        StartSpawning();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Auto Start disabled - call StartSpawning() manually"));
    }
}

void AEnemySpawner::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    
    CleanupDeadEnemies();
}

void AEnemySpawner::StartSpawning()
{
    UE_LOG(LogTemp, Warning, TEXT("=== StartSpawning Called ==="));

    if (!EnemyClass)
    {
        UE_LOG(LogTemp, Error, TEXT("✗ EnemyClass is NULL! Cannot spawn enemies!"));
        UE_LOG(LogTemp, Error, TEXT("   Set Enemy Class in BP_EnemySpawner to BP_Enemy"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("✓ Enemy Spawner Started!"));
    UE_LOG(LogTemp, Warning, TEXT("   Will spawn: %s"), *EnemyClass->GetName());
    UE_LOG(LogTemp, Warning, TEXT("   Every %.1f seconds"), SpawnInterval);

    GetWorld()->GetTimerManager().SetTimer(
        SpawnTimerHandle,
        this,
        &AEnemySpawner::SpawnEnemy,
        SpawnInterval,
        true
    );

  
    SpawnEnemy();
}

void AEnemySpawner::StopSpawning()
{
    UE_LOG(LogTemp, Warning, TEXT("Enemy Spawner Stopped!"));

    GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
}

void AEnemySpawner::RegisterEnemy(AEnemyBase* Enemy)
{
    if (Enemy && IsValid(Enemy))
    {
        AliveEnemies.Add(Enemy);
        UE_LOG(LogTemp, Log, TEXT("Registered external enemy: %s (Total: %d)"),
            *Enemy->GetName(), AliveEnemies.Num());
    }
}



void AEnemySpawner::SpawnEnemy()
{
    UE_LOG(LogTemp, Warning, TEXT("=== SpawnEnemy Called ==="));

    if (AliveEnemies.Num() >= MaxEnemies)
    {
        UE_LOG(LogTemp, Log, TEXT("Max enemies reached (%d/%d), skipping spawn"),
            AliveEnemies.Num(), MaxEnemies);
        return;
    }

    if (!EnemyClass)
    {
        UE_LOG(LogTemp, Error, TEXT("✗ EnemyClass is NULL!"));
        return;
    }

    FVector SpawnLocation = GetRandomSpawnLocation();
    FRotator SpawnRotation = FRotator::ZeroRotator;

    TSubclassOf<AEnemyBase> ChosenClass = SelectEnemyClass();
    UE_LOG(LogTemp, Warning, TEXT("Attempting to spawn %s at: %s"),
        *ChosenClass->GetName(), *SpawnLocation.ToString());


    UE_LOG(LogTemp, Warning, TEXT("Attempting to spawn enemy at: %s"), *SpawnLocation.ToString());

    AEnemyBase* NewEnemy = GetWorld()->SpawnActor<AEnemyBase>(
        ChosenClass,
        SpawnLocation,
        SpawnRotation
    );

    if (NewEnemy)
    {
        AliveEnemies.Add(NewEnemy);
        UE_LOG(LogTemp, Warning, TEXT("✓ Successfully spawned enemy! (Total: %d/%d)"),
            AliveEnemies.Num(), MaxEnemies);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("✗ SpawnActor returned NULL!"));
    }
}



FVector AEnemySpawner::GetRandomSpawnLocation()
{
    
    FVector Center = GetActorLocation();

    float RandomAngle = FMath::FRandRange(0.0f, 360.0f);
    float RandomDistance = FMath::FRandRange(SpawnRadius * 0.5f, SpawnRadius);

    float X = Center.X + RandomDistance * FMath::Cos(FMath::DegreesToRadians(RandomAngle));
    float Y = Center.Y + RandomDistance * FMath::Sin(FMath::DegreesToRadians(RandomAngle));

    FVector RandomLocation = FVector(X, Y, Center.Z);

    
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (NavSys)
    {
        FNavLocation NavLocation;
        if (NavSys->ProjectPointToNavigation(RandomLocation, NavLocation, FVector(500, 500, 500)))
        {
            return NavLocation.Location;
        }
    }

    
    return RandomLocation;
}

void AEnemySpawner::CleanupDeadEnemies()
{
   
    AliveEnemies.RemoveAll([](AEnemyBase* Enemy)
        {
            return Enemy == nullptr || Enemy->IsDead() || !IsValid(Enemy);
        });
}


TSubclassOf<AEnemyBase> AEnemySpawner::SelectEnemyClass() const
{
    
    TArray<TSubclassOf<AEnemyBase>> Pool;

   
    if (EnemyClass)
    {
        int32 BaseWeight = FMath::Max(1, 6 - CurrentWave);
        for (int32 i = 0; i < BaseWeight; i++)
            Pool.Add(EnemyClass);
    }

  
    if (FastEnemyClass && CurrentWave >= 2)
    {
        int32 FastWeight = FMath::Min(CurrentWave - 1, 3);
        for (int32 i = 0; i < FastWeight; i++)
            Pool.Add(FastEnemyClass);
    }

    
    if (TankEnemyClass && CurrentWave >= 4)
    {
        int32 TankWeight = FMath::Min(CurrentWave / 4, 2);
        for (int32 i = 0; i < TankWeight; i++)
            Pool.Add(TankEnemyClass);
    }

    
    if (Pool.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("SelectEnemyClass: Pool is empty! Falling back to EnemyClass"));
        return EnemyClass;
    }

    
    UE_LOG(LogTemp, Log, TEXT("Spawn pool size: %d (Wave %d)"), Pool.Num(), CurrentWave);

    return Pool[FMath::RandRange(0, Pool.Num() - 1)];
}