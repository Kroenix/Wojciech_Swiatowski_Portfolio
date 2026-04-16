#include "WaveManager.h"
#include "EnemyBase.h"
#include "BossEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "MyArenaCharacter.h"
#include "GameHUD.h"

AWaveManager::AWaveManager()
{
    PrimaryActorTick.bCanEverTick = true;

    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;
}

void AWaveManager::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("=== Wave Manager Initialized ==="));

    if (bAutoStart)
    {
        StartWaveSystem();
    }
}

void AWaveManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bWaveActive)
    {
        
        int32 CurrentAlive = 0;
        for (int32 i = AliveEnemies.Num() - 1; i >= 0; i--)
        {
            if (!AliveEnemies[i] || AliveEnemies[i]->IsDead() || !IsValid(AliveEnemies[i]))
            {
                AliveEnemies.RemoveAt(i);
            }
            else
            {
                CurrentAlive++;
            }
        }

        
        int32 Killed = EnemiesSpawnedThisWave - CurrentAlive;
        int32 Remaining = CurrentAlive;
        int32 Total = EnemiesToSpawnThisWave;

      
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PC)
        {
            AMyArenaCharacter* Character = Cast<AMyArenaCharacter>(PC->GetPawn());
            if (Character && Character->GetGameHUD())
            {
                Character->GetGameHUD()->UpdateWaveProgress(Killed, Remaining, Total);
            }
        }

        CheckWaveComplete();
    }
}

void AWaveManager::StartWaveSystem()
{
    UE_LOG(LogTemp, Warning, TEXT("=== WAVE SYSTEM STARTED ==="));
    UE_LOG(LogTemp, Warning, TEXT("First wave starts in %.1f seconds..."), FirstWaveDelay);

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC) return;

    AMyArenaCharacter* Character = Cast<AMyArenaCharacter>(PC->GetPawn());
    if (!Character || !Character->GetGameHUD()) return;

 
    Character->GetGameHUD()->ShowAnnouncement(TEXT("GET READY!"), 2.5f, true);


    if (FirstWaveDelay >= 4.5f)
    {
        float TimeFor3 = FirstWaveDelay - 3.0f;
        float TimeFor2 = FirstWaveDelay - 2.0f;
        float TimeFor1 = FirstWaveDelay - 1.0f;

       
        GetWorld()->GetTimerManager().SetTimer(Countdown3TimerHandle, [Character]()
            {
                if (Character && Character->GetGameHUD())
                {
                    UE_LOG(LogTemp, Error, TEXT(">>> COUNTDOWN: 3 <<<"));
                    Character->GetGameHUD()->ShowAnnouncement(TEXT("3"), 0.9f, true);
                }
            }, TimeFor3, false);

      
        GetWorld()->GetTimerManager().SetTimer(Countdown2TimerHandle, [Character]()
            {
                if (Character && Character->GetGameHUD())
                {
                    UE_LOG(LogTemp, Error, TEXT(">>> COUNTDOWN: 2 <<<"));
                    Character->GetGameHUD()->ShowAnnouncement(TEXT("2"), 0.9f, true);
                }
            }, TimeFor2, false);

        
        GetWorld()->GetTimerManager().SetTimer(Countdown1TimerHandle, [Character]()
            {
                if (Character && Character->GetGameHUD())
                {
                    UE_LOG(LogTemp, Error, TEXT(">>> COUNTDOWN: 1 <<<"));
                    Character->GetGameHUD()->ShowAnnouncement(TEXT("1"), 0.9f, true);
                }
            }, TimeFor1, false);
    }

   
    GetWorld()->GetTimerManager().SetTimer(WaveStartTimerHandle, [this]()
        {
            StartWave(2);// Do Zmiany
        }, FirstWaveDelay, false);
}

bool AWaveManager::IsBossWave(int32 WaveNumber) const
{
    bool bResult = (WaveNumber % BossWaveInterval == 0);
    UE_LOG(LogTemp, Log, TEXT("IsBossWave(%d): %s (Interval: %d)"),
        WaveNumber, bResult ? TEXT("TRUE") : TEXT("FALSE"), BossWaveInterval);
    return bResult;
}

void AWaveManager::SpawnBossEnemy()
{
    UE_LOG(LogTemp, Error, TEXT("=== SpawnBossEnemy() called ==="));

    if (!BossEnemyClass)
    {
        UE_LOG(LogTemp, Error, TEXT("✗ BossEnemyClass is NULL!"));
        UE_LOG(LogTemp, Error, TEXT("Spawning normal enemy instead..."));
        SpawnEnemy();
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("✓ BossEnemyClass is set: %s"), *BossEnemyClass->GetName());

    FVector SpawnLocation = GetRandomSpawnLocation();
    FRotator SpawnRotation = FRotator::ZeroRotator;

    UE_LOG(LogTemp, Warning, TEXT("Attempting to spawn boss at location: %s"), *SpawnLocation.ToString());

    ABossEnemy* Boss = GetWorld()->SpawnActor<ABossEnemy>(BossEnemyClass, SpawnLocation, SpawnRotation);

    if (Boss)
    {
        if (!Boss->MinionClass)
        {
            UE_LOG(LogTemp, Warning, TEXT("Boss MinionClass not set, using EnemyClass from WaveManager"));
            Boss->MinionClass = EnemyClass; 

            if (Boss->MinionClass)
            {
                UE_LOG(LogTemp, Warning, TEXT("✓ Boss MinionClass set to: %s"), *Boss->MinionClass->GetName());
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("✗ Failed to set Boss MinionClass - Spawn Minions will fail!"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("✓ Boss already has MinionClass: %s"), *Boss->MinionClass->GetName());
        }

        AliveEnemies.Add(Boss);
        EnemiesSpawnedThisWave++;

        UE_LOG(LogTemp, Error, TEXT(">>> ✓ BOSS ENEMY SPAWNED SUCCESSFULLY <<<"));

        
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PC)
        {
            AMyArenaCharacter* Character = Cast<AMyArenaCharacter>(PC->GetPawn());
            if (Character && Character->GetGameHUD())
            {
                FTimerHandle BossAnnouncement;
                GetWorld()->GetTimerManager().SetTimer(BossAnnouncement, [Character]()
                    {
                        if (Character && Character->GetGameHUD())
                        {
                            Character->GetGameHUD()->ShowAnnouncement(TEXT("BOSS APPEARED!"), 2.5f, true);
                        }
                    }, 1.0f, false);
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("✗ FAILED TO SPAWN BOSS! SpawnActor returned nullptr"));
    }
}

void AWaveManager::StartWave(int32 WaveNumber)
{

    UE_LOG(LogTemp, Error, TEXT("WaveManager EnemyClass: %s"),
        EnemyClass ? *EnemyClass->GetName() : TEXT("NULL"));
    UE_LOG(LogTemp, Error, TEXT("WaveManager FastEnemyClass: %s"),
        FastEnemyClass ? *FastEnemyClass->GetName() : TEXT("NULL"));

    CurrentWave = WaveNumber;
    bWaveActive = true;
    EnemiesSpawnedThisWave = 0;
    EnemiesToSpawnThisWave = CalculateEnemiesForWave(WaveNumber);

    bool bIsBoss = IsBossWave(WaveNumber);

    UE_LOG(LogTemp, Error, TEXT("========================================"));
    UE_LOG(LogTemp, Error, TEXT("     WAVE %d STARTED"), CurrentWave);
    UE_LOG(LogTemp, Error, TEXT("     Is Boss Wave: %s"), bIsBoss ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Error, TEXT("     Enemies: %d"), EnemiesToSpawnThisWave);
    UE_LOG(LogTemp, Error, TEXT("========================================"));

    
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC)
    {
        AMyArenaCharacter* Character = Cast<AMyArenaCharacter>(PC->GetPawn());
        if (Character && Character->GetGameHUD())
        {
            
            Character->GetGameHUD()->UpdateWave(CurrentWave);

            
            FString WaveText;

            if (IsBossWave(CurrentWave))
            {
               
                WaveText = FString::Printf(TEXT(">>> BOSS WAVE %d! <<<"), CurrentWave);
            }
            else if (CurrentWave == 1)
            {
                WaveText = TEXT("=== WAVE 1 - BEGIN! ===");
            }
            else if (CurrentWave % 10 == 0)
            {
                WaveText = FString::Printf(TEXT(">>> WAVE %d - LEGENDARY! <<<"), CurrentWave);
            }
            else if (CurrentWave == 3)
            {
                WaveText = TEXT("=== WAVE 3 - GETTING TOUGH! ===");
            }
            else
            {
                WaveText = FString::Printf(TEXT("=== WAVE %d ==="), CurrentWave);
            }

            Character->GetGameHUD()->ShowAnnouncement(WaveText, 3.0f, true);
        }
    }

   
    if (IsBossWave(CurrentWave))
    {
        UE_LOG(LogTemp, Error, TEXT(">>> ATTEMPTING TO SPAWN BOSS <<<"));
        SpawnBossEnemy();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Not a boss wave, skipping boss spawn"));
    }

   
    GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &AWaveManager::SpawnEnemy, SpawnInterval, true);

   
    if (bSpawnBossWithNormalEnemies || !IsBossWave(CurrentWave))
    {
        SpawnEnemy();
    }
}

void AWaveManager::SpawnEnemy()
{
    if (EnemiesSpawnedThisWave >= EnemiesToSpawnThisWave)
    {
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
        UE_LOG(LogTemp, Log, TEXT("All enemies for wave %d spawned"), CurrentWave);
        return;
    }

    if (!EnemyClass)
    {
        UE_LOG(LogTemp, Error, TEXT("EnemyClass not set!"));
        return;
    }

    FVector SpawnLocation = GetRandomSpawnLocation();
    FRotator SpawnRotation = FRotator::ZeroRotator;

   
    TSubclassOf<AEnemyBase> ChosenClass = SelectEnemyClass();

    AEnemyBase* NewEnemy = GetWorld()->SpawnActor<AEnemyBase>(
        ChosenClass,
        SpawnLocation,
        SpawnRotation
    );

    if (NewEnemy)
    {
        AliveEnemies.Add(NewEnemy);
        EnemiesSpawnedThisWave++;

        UE_LOG(LogTemp, Log, TEXT("Spawned %s (%d/%d for wave %d)"),
            *ChosenClass->GetName(),
            EnemiesSpawnedThisWave,
            EnemiesToSpawnThisWave,
            CurrentWave);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("✗ SpawnActor returned NULL for %s"),
            *ChosenClass->GetName());
    }
}

void AWaveManager::CheckWaveComplete()
{
   
    if (EnemiesSpawnedThisWave >= EnemiesToSpawnThisWave && AliveEnemies.Num() == 0)
    {
        CompleteWave();
    }
}

void AWaveManager::CompleteWave()
{
    bWaveActive = false;

    bool bWasBossWave = IsBossWave(CurrentWave);

    UE_LOG(LogTemp, Warning, TEXT("========================================"));
    if (bWasBossWave)
    {
        UE_LOG(LogTemp, Warning, TEXT("     BOSS WAVE %d COMPLETE!"), CurrentWave);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("     WAVE %d COMPLETE!"), CurrentWave);
    }
    UE_LOG(LogTemp, Warning, TEXT("========================================"));


    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC)
    {
        AMyArenaCharacter* Character = Cast<AMyArenaCharacter>(PC->GetPawn());
        if (Character && Character->GetGameHUD())
        {
        
            FString CompleteText;
            if (bWasBossWave)
            {
                CompleteText = TEXT(">>> BOSS DEFEATED! <<<");
            }
            else
            {
                CompleteText = FString::Printf(TEXT("WAVE %d COMPLETE!"), CurrentWave);
            }

            Character->GetGameHUD()->ShowAnnouncement(CompleteText, 3.0f, true);

           
            int32 BonusScore = CurrentWave * 50;
            if (bWasBossWave)
            {
                BonusScore *= 3; 
            }

            Character->AddScore(BonusScore);
            UE_LOG(LogTemp, Log, TEXT("Wave bonus: +%d points"), BonusScore);

          
            FTimerHandle NextWaveNotification;
            GetWorld()->GetTimerManager().SetTimer(NextWaveNotification, [this, Character]()
                {
                    if (Character && Character->GetGameHUD())
                    {
                        FString NextWaveText = FString::Printf(TEXT("Next wave in %.0f seconds..."),
                            TimeBetweenWaves - 3.0f);
                        Character->GetGameHUD()->ShowAnnouncement(NextWaveText, 2.0f, false);
                    }
                }, 3.0f, false);
        }
    }

    
    GetWorld()->GetTimerManager().SetTimer(WaveStartTimerHandle, [this]()
        {
            StartNextWave();
        }, TimeBetweenWaves, false);
}

void AWaveManager::StartNextWave()
{
    StartWave(CurrentWave + 1);
}

int32 AWaveManager::CalculateEnemiesForWave(int32 WaveNumber)
{
   
    return StartingEnemiesPerWave + ((WaveNumber - 1) * EnemiesIncreasePerWave);
}

int32 AWaveManager::GetEnemiesRemaining() const
{
    return AliveEnemies.Num();
}

FVector AWaveManager::GetRandomSpawnLocation()
{
    FVector Center = GetActorLocation();

    float RandomAngle = FMath::FRandRange(0.0f, 360.0f);
    float RandomDistance = FMath::FRandRange(SpawnRadius * 0.5f, SpawnRadius);

    float X = Center.X + RandomDistance * FMath::Cos(FMath::DegreesToRadians(RandomAngle));
    float Y = Center.Y + RandomDistance * FMath::Sin(FMath::DegreesToRadians(RandomAngle));

    return FVector(X, Y, Center.Z);
}

void AWaveManager::CleanupDeadEnemies()
{
    AliveEnemies.RemoveAll([](AEnemyBase* Enemy)
        {
            return Enemy == nullptr || Enemy->IsDead() || !IsValid(Enemy);
        });
}

TSubclassOf<AEnemyBase> AWaveManager::SelectEnemyClass() const
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
        UE_LOG(LogTemp, Error, TEXT("SelectEnemyClass: pool empty, falling back to EnemyClass"));
        return EnemyClass;
    }

    return Pool[FMath::RandRange(0, Pool.Num() - 1)];
}

void AWaveManager::RegisterEnemy(AEnemyBase* Enemy)
{
    if (Enemy && IsValid(Enemy))
    {
        AliveEnemies.Add(Enemy);
        EnemiesSpawnedThisWave++;

        UE_LOG(LogTemp, Log, TEXT("Registered external enemy: %s (Wave total: %d)"),
            *Enemy->GetName(), EnemiesSpawnedThisWave);
    }
}