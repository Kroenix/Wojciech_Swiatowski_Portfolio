
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaveManager.generated.h"

UCLASS()
class ARENA_API AWaveManager : public AActor
{
	GENERATED_BODY()
	
public:	
    AWaveManager();
    virtual void Tick(float DeltaTime) override;

   
    UFUNCTION(BlueprintCallable, Category = "Waves")
    void StartWaveSystem();

  
    UFUNCTION(BlueprintPure, Category = "Waves")
    int32 GetCurrentWave() const { return CurrentWave; }

    UFUNCTION(BlueprintPure, Category = "Waves")
    int32 GetEnemiesRemaining() const;

    UFUNCTION(BlueprintPure, Category = "Waves")
    bool IsWaveActive() const { return bWaveActive; }

    UFUNCTION(BlueprintPure, Category = "Waves")
    float GetTimeBetweenWaves() const { return TimeBetweenWaves; }

    UPROPERTY(EditDefaultsOnly, Category = "Waves")
    TSubclassOf<class AEnemyBase> EnemyClass;

    UPROPERTY(EditDefaultsOnly, Category = "Waves")
    TSubclassOf<class ABossEnemy> BossEnemyClass;

    UPROPERTY(EditDefaultsOnly, Category = "Waves")
    TSubclassOf<class AEnemyBase> FastEnemyClass;

    UPROPERTY(EditDefaultsOnly, Category = "Waves")
    TSubclassOf<class AEnemyBase> TankEnemyClass;

  
    UFUNCTION(BlueprintCallable, Category = "Waves")
    void RegisterEnemy(AEnemyBase* Enemy);

    UPROPERTY(EditDefaultsOnly, Category = "Waves")
    int32 BossWaveInterval = 5; 

    UPROPERTY(EditDefaultsOnly, Category = "Waves")
    bool bSpawnBossWithNormalEnemies = true; 

   

protected:
    virtual void BeginPlay() override;

 
    UPROPERTY(EditDefaultsOnly, Category = "Waves")
    int32 StartingEnemiesPerWave = 5;

    UPROPERTY(EditDefaultsOnly, Category = "Waves")
    int32 EnemiesIncreasePerWave = 3;

    UPROPERTY(EditDefaultsOnly, Category = "Waves")
    float TimeBetweenWaves = 10.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Waves")
    float FirstWaveDelay = 5.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Waves")
    bool bAutoStart = true;

  
    

    UPROPERTY(EditDefaultsOnly, Category = "Waves")
    float SpawnRadius = 1500.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Waves")
    float SpawnInterval = 2.0f;

    

private:
  
    int32 CurrentWave = 0;
    int32 EnemiesToSpawnThisWave = 0;
    int32 EnemiesSpawnedThisWave = 0;
    int32 EnemiesKilledThisWave = 0;
    bool bWaveActive = false;
    bool IsBossWave(int32 WaveNumber) const;
    void SpawnBossEnemy();
  
    UPROPERTY()
    TArray<class AEnemyBase*> AliveEnemies;

   
    FTimerHandle SpawnTimerHandle;
    FTimerHandle WaveStartTimerHandle;
    FTimerHandle Countdown3TimerHandle; 
    FTimerHandle Countdown2TimerHandle;
    FTimerHandle Countdown1TimerHandle;

  
    void StartWave(int32 WaveNumber);
    TSubclassOf<AEnemyBase> SelectEnemyClass() const;

    void SpawnEnemy();
    void CheckWaveComplete();
    void CompleteWave();
    void StartNextWave();

  
    FVector GetRandomSpawnLocation();
    void CleanupDeadEnemies();
    int32 CalculateEnemiesForWave(int32 WaveNumber);

 

};
