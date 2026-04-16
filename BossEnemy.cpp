#include "BossEnemy.h"


#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraShakeBase.h"


#include "Kismet/GameplayStatics.h"
#include "MyArenaCharacter.h"
#include "TimerManager.h"
#include "GameHUD.h"
#include "EnemySpawner.h"
#include "WaveManager.h"


ABossEnemy::ABossEnemy()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ABossEnemy::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Error, TEXT("========================================"));
    UE_LOG(LogTemp, Error, TEXT("   BOSS SPAWNED!"));
    UE_LOG(LogTemp, Error, TEXT("========================================"));

    
    MaxHealth = MaxHealth * HealthMultiplier;
    CurrentHealth = MaxHealth;

    UE_LOG(LogTemp, Warning, TEXT("Boss Health: %.0f"), MaxHealth);

    
    SetActorScale3D(FVector(SizeMultiplier));

    OriginalBossColor = BossColor;

   
    if (BodyMesh)
    {
        UMaterialInstanceDynamic* BossMaterial = BodyMesh->CreateDynamicMaterialInstance(0);
        if (BossMaterial)
        {
            BossMaterial->SetVectorParameterValue(FName("Color"), BossColor);
            BodyMaterialInstance = BossMaterial;
        }
    }

    
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed * SpeedMultiplier;
        UE_LOG(LogTemp, Log, TEXT("Boss Speed: %.0f"), GetCharacterMovement()->MaxWalkSpeed);
    }


    APlayerController* PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    if (PC)
    {
        AMyArenaCharacter* Player = Cast<AMyArenaCharacter>(PC->GetPawn());
        if (Player && Player->GetGameHUD())
        {
            FString BossName = TEXT(">>> BOSS <<<");
            Player->GetGameHUD()->ShowBossHealthBar(BossName);
            Player->GetGameHUD()->UpdateBossHealth(CurrentHealth, MaxHealth);
        }
    }


    GetWorld()->GetTimerManager().SetTimer(AbilityTimerHandle, this, &ABossEnemy::TryUseAbility, 1.0f, true);
    LastAbilityTime = GetWorld()->GetTimeSeconds();


}

void ABossEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);


    if (bIsCharging)
    {
        UpdateChargeAttack(DeltaTime);
    }

  
    if (bIsShielded)
    {
        UpdateShield(DeltaTime);
    }
}

void ABossEnemy::TakeDamage(float DamageAmount)
{
    if (bIsDead)
    {
        return;
    }


    if (bIsShielded)
    {
        UE_LOG(LogTemp, Warning, TEXT("Boss shield blocked %.0f damage!"), DamageAmount);

       
        if (BodyMaterialInstance)
        {
            BodyMaterialInstance->SetVectorParameterValue(FName("Color"), FLinearColor::Blue);

            FTimerHandle FlashTimer;
            GetWorld()->GetTimerManager().SetTimer(FlashTimer, [this]()
                {
                    if (BodyMaterialInstance && !bIsDead)
                    {
                        BodyMaterialInstance->SetVectorParameterValue(FName("Color"),
                            bIsShielded ? FLinearColor(0.0f, 1.0f, 1.0f) : OriginalBossColor);
                    }
                }, 0.1f, false);
        }
        return;
    }


    CurrentHealth -= DamageAmount;

    UE_LOG(LogTemp, Log, TEXT("Boss HP: %.0f/%.0f"), CurrentHealth, MaxHealth);

    
    APlayerController* PC = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    if (PC)
    {
        AMyArenaCharacter* Player = Cast<AMyArenaCharacter>(PC->GetPawn());
        if (Player && Player->GetGameHUD())
        {
            Player->GetGameHUD()->UpdateBossHealth(CurrentHealth, MaxHealth);
        }
    }

    if (HitSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
    }

    FlashDamageEffect();


    float HealthPercent = CurrentHealth / MaxHealth;
    if (!bIsEnraged && HealthPercent <= EnrageHealthPercent)
    {
        EnterEnrageMode();
    }

    if (CurrentHealth <= 0)
    {
        CurrentHealth = 0;
        Die();
    }
}

void ABossEnemy::FlashDamageEffect()
{
    if (BodyMaterialInstance)
    {
        BodyMaterialInstance->SetVectorParameterValue(FName("Color"), FLinearColor::White);

        FTimerHandle FlashTimer;
        GetWorld()->GetTimerManager().SetTimer(FlashTimer, [this]()
            {
                if (BodyMaterialInstance && !bIsDead)
                {
                    FLinearColor RestoreColor = OriginalBossColor;
                    if (bIsEnraged)
                    {
                        RestoreColor = FLinearColor(1.0f, 0.0f, 0.0f); 
                    }
                    else if (bIsShielded)
                    {
                        RestoreColor = FLinearColor(0.0f, 1.0f, 1.0f);
                    }

                    BodyMaterialInstance->SetVectorParameterValue(FName("Color"), RestoreColor);
                }
            }, 0.1f, false);
    }
}

bool ABossEnemy::CanUseAbility() const
{
    if (bIsDead || bIsCharging || bIsShielded)
    {
        return false;
    }

    float TimeSinceLastAbility = GetWorld()->GetTimeSeconds() - LastAbilityTime;
    return TimeSinceLastAbility >= AbilityCooldown;
}

void ABossEnemy::TryUseAbility()
{
    if (!CanUseAbility())
    {
        return;
    }

   
    AMyArenaCharacter* Player = GetPlayerCharacter();
    if (!Player || Player->IsDead())
    {
        return;
    }

    UseRandomAbility();
    LastAbilityTime = GetWorld()->GetTimeSeconds();
}

void ABossEnemy::UseRandomAbility()
{
    TArray<EBossAbility> AvailableAbilities;

    if (bCanSpawnMinions) AvailableAbilities.Add(EBossAbility::SpawnMinions);
    if (bCanUseAOE) AvailableAbilities.Add(EBossAbility::AOEAttack);
    if (bCanCharge) AvailableAbilities.Add(EBossAbility::ChargeAttack);
    if (bCanUseShield) AvailableAbilities.Add(EBossAbility::Shield);

    if (AvailableAbilities.Num() == 0)
    {
        return;
    }

    int32 RandomIndex = FMath::RandRange(0, AvailableAbilities.Num() - 1);
    EBossAbility SelectedAbility = AvailableAbilities[RandomIndex];

    switch (SelectedAbility)
    {
    case EBossAbility::SpawnMinions:
        SpawnMinions();
        break;
    case EBossAbility::AOEAttack:
        UseAOEAttack();
        break;
    case EBossAbility::ChargeAttack:
        StartChargeAttack();
        break;
    case EBossAbility::Shield:
        ActivateShield();
        break;
    default:
        break;
    }
}

void ABossEnemy::SpawnMinions()
{
    UE_LOG(LogTemp, Error, TEXT(">>> BOSS SPAWNING MINIONS! <<<"));

  
    if (!MinionClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("MinionClass not set, using default enemy"));
        MinionClass = AEnemyBase::StaticClass();
    }

  
    AWaveManager* WaveManager = Cast<AWaveManager>(
        UGameplayStatics::GetActorOfClass(GetWorld(), AWaveManager::StaticClass()));

    if (!WaveManager)
    {
        UE_LOG(LogTemp, Error, TEXT("SpawnMinions: WaveManager not found — minions won't count toward wave"));
    }

    int32 MinionsToSpawn = bIsEnraged ? 5 : 3;

    UE_LOG(LogTemp, Warning, TEXT("Spawning %d minions (enraged: %s)"),
        MinionsToSpawn, bIsEnraged ? TEXT("YES") : TEXT("NO"));

    int32 SuccessfulSpawns = 0;

    for (int32 i = 0; i < MinionsToSpawn; i++)
    {
       
        float Angle = (360.0f / MinionsToSpawn) * i;
        float Distance = 300.0f;

        FVector SpawnOffset = FVector(
            FMath::Cos(FMath::DegreesToRadians(Angle)) * Distance,
            FMath::Sin(FMath::DegreesToRadians(Angle)) * Distance,
            0.0f
        );

        FVector SpawnLocation = GetActorLocation() + SpawnOffset;

        AEnemyBase* Minion = GetWorld()->SpawnActor<AEnemyBase>(
            MinionClass,
            SpawnLocation,
            FRotator::ZeroRotator
        );

        if (Minion)
        {
            
            if (WaveManager)
            {
                WaveManager->RegisterEnemy(Minion);
            }

            SuccessfulSpawns++;
            UE_LOG(LogTemp, Log, TEXT("Minion %d/%d spawned and registered"),
                SuccessfulSpawns, MinionsToSpawn);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn minion %d"), i + 1);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Minion spawn complete: %d/%d succeeded"),
        SuccessfulSpawns, MinionsToSpawn);

  
    AMyArenaCharacter* Player = GetPlayerCharacter();
    if (Player && Player->GetGameHUD())
    {
        Player->GetGameHUD()->ShowAnnouncement(
            TEXT("BOSS SUMMONED MINIONS!"), 2.0f, false);
    }
}

void ABossEnemy::UseAOEAttack()
{
    UE_LOG(LogTemp, Error, TEXT(">>> BOSS AOE ATTACK! <<<"));

    float AOERadius = 500.0f;
    float AOEDamage = bIsEnraged ? 30.0f : 20.0f;

    if (AbilityCameraShake)
    {
        APlayerController* PC = Cast<APlayerController>(
            UGameplayStatics::GetPlayerController(GetWorld(), 0)
        );
        if (PC && PC->PlayerCameraManager)
        {
            PC->PlayerCameraManager->StartCameraShake(AbilityCameraShake, 1.5f);
        }
    }

   
    AMyArenaCharacter* Player = GetPlayerCharacter();
    if (Player)
    {
        float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());

        if (Distance <= AOERadius)
        {
            Player->TakeDamage(AOEDamage);
            UE_LOG(LogTemp, Warning, TEXT("Player hit by AOE for %.0f damage!"), AOEDamage);
        }
    }

  
    if (BodyMaterialInstance)
    {
        SetActorScale3D(FVector(SizeMultiplier * 1.3f));
        BodyMaterialInstance->SetVectorParameterValue(FName("Color"), FLinearColor::Red);

        FTimerHandle EffectTimer;
        GetWorld()->GetTimerManager().SetTimer(EffectTimer, [this]()
            {
                SetActorScale3D(FVector(SizeMultiplier));
                if (BodyMaterialInstance)
                {
                    BodyMaterialInstance->SetVectorParameterValue(FName("Color"),
                        bIsEnraged ? FLinearColor(1.0f, 0.0f, 0.0f) : OriginalBossColor);
                }
            }, 0.3f, false);
    }

   
    if (Player && Player->GetGameHUD())
    {
        Player->GetGameHUD()->ShowAnnouncement(TEXT("BOSS AOE ATTACK!"), 1.5f, false);
    }
}

void ABossEnemy::StartChargeAttack()
{
    UE_LOG(LogTemp, Error, TEXT(">>> BOSS CHARGING! <<<"));

    AMyArenaCharacter* Player = GetPlayerCharacter();
    if (!Player)
    {
        return;
    }

    bIsCharging = true;
    ChargeStartLocation = GetActorLocation();
    ChargeTargetLocation = Player->GetActorLocation();
    ChargeProgress = 0.0f;

  
    if (BodyMaterialInstance)
    {
        BodyMaterialInstance->SetVectorParameterValue(FName("Color"), FLinearColor::Yellow);
    }

 
    if (Player->GetGameHUD())
    {
        Player->GetGameHUD()->ShowAnnouncement(TEXT("BOSS IS CHARGING!"), 1.5f, false);
    }
}

void ABossEnemy::UpdateChargeAttack(float DeltaTime)
{
    ChargeProgress += DeltaTime * 3.0f; 

    if (ChargeProgress >= 1.0f)
    {
        
        bIsCharging = false;
        ChargeProgress = 0.0f;

       
        if (BodyMaterialInstance)
        {
            BodyMaterialInstance->SetVectorParameterValue(FName("Color"),
                bIsEnraged ? FLinearColor(1.0f, 0.0f, 0.0f) : OriginalBossColor);
        }

       
        AMyArenaCharacter* Player = GetPlayerCharacter();
        if (Player)
        {
            float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
            if (Distance <= 200.0f)
            {
                float ChargeDamage = bIsEnraged ? 40.0f : 25.0f;
                Player->TakeDamage(ChargeDamage);
                UE_LOG(LogTemp, Warning, TEXT("Boss charge hit player for %.0f damage!"), ChargeDamage);
            }
        }

        UE_LOG(LogTemp, Log, TEXT("Boss charge complete"));
        return;
    }

    
    FVector NewLocation = FMath::Lerp(ChargeStartLocation, ChargeTargetLocation, ChargeProgress);
    SetActorLocation(NewLocation);
}

void ABossEnemy::ActivateShield()
{
    UE_LOG(LogTemp, Error, TEXT(">>> BOSS SHIELD ACTIVATED! <<<"));

    bIsShielded = true;

    
    if (BodyMaterialInstance)
    {
        BodyMaterialInstance->SetVectorParameterValue(FName("Color"), FLinearColor(0.0f, 1.0f, 1.0f)); 
    }

   
    float ShieldDuration = bIsEnraged ? 3.0f : 5.0f;

    FTimerHandle ShieldTimer;
    GetWorld()->GetTimerManager().SetTimer(ShieldTimer, [this]()
        {
            bIsShielded = false;

            if (BodyMaterialInstance)
            {
                BodyMaterialInstance->SetVectorParameterValue(FName("Color"),
                    bIsEnraged ? FLinearColor(1.0f, 0.0f, 0.0f) : OriginalBossColor);
            }

            UE_LOG(LogTemp, Log, TEXT("Boss shield expired"));

        }, ShieldDuration, false);

    
    AMyArenaCharacter* Player = GetPlayerCharacter();
    if (Player && Player->GetGameHUD())
    {
        Player->GetGameHUD()->ShowAnnouncement(TEXT("BOSS SHIELD ACTIVE!"), 2.0f, false);
    }
}

void ABossEnemy::UpdateShield(float DeltaTime)
{
   
    if (BodyMaterialInstance)
    {
        float PulseValue = (FMath::Sin(GetWorld()->GetTimeSeconds() * 5.0f) + 1.0f) * 0.5f;
        FLinearColor ShieldColor = FLinearColor(0.0f, 1.0f, 1.0f) * (0.7f + PulseValue * 0.3f); 
        BodyMaterialInstance->SetVectorParameterValue(FName("Color"), ShieldColor);
    }
}

void ABossEnemy::EnterEnrageMode()
{
    if (bIsEnraged)
    {
        return;
    }

    bIsEnraged = true;

    UE_LOG(LogTemp, Error, TEXT("========================================"));
    UE_LOG(LogTemp, Error, TEXT("   BOSS ENRAGED!!!"));
    UE_LOG(LogTemp, Error, TEXT("========================================"));

    
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed *= 1.5f;
        UE_LOG(LogTemp, Warning, TEXT("Boss speed increased to %.0f"), GetCharacterMovement()->MaxWalkSpeed);
    }

    
    OriginalBossColor = FLinearColor(1.0f, 0.0f, 0.0f);
    if (BodyMaterialInstance)
    {
        BodyMaterialInstance->SetVectorParameterValue(FName("Color"), OriginalBossColor);
    }

    
    AbilityCooldown = AbilityCooldown * 0.5f;

    
    SetActorScale3D(FVector(SizeMultiplier * 1.2f));

    
    AMyArenaCharacter* Player = GetPlayerCharacter();
    if (Player && Player->GetGameHUD())
    {
        Player->GetGameHUD()->ShowAnnouncement(TEXT(">>> BOSS ENRAGED! <<<"), 3.0f, true);
    }
}

AMyArenaCharacter* ABossEnemy::GetPlayerCharacter()
{
    return Cast<AMyArenaCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}



void ABossEnemy::Die()
{
    
    GetWorld()->GetTimerManager().ClearTimer(AbilityTimerHandle);

    
    APlayerController* PC = Cast<APlayerController>(
        UGameplayStatics::GetPlayerController(GetWorld(), 0));
    if (PC)
    {
        AMyArenaCharacter* Player = Cast<AMyArenaCharacter>(PC->GetPawn());
        if (Player && Player->GetGameHUD())
        {
            Player->GetGameHUD()->HideBossHealthBar();
        }
    }

    
    ScoreValue = ScoreValue * BonusScoreMultiplier;

    
    Super::Die();
}