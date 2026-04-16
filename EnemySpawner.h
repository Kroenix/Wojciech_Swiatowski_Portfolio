/*#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class ARENA_API AEnemySpawner : public AActor
{
    GENERATED_BODY()

public:
    AEnemySpawner();
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Spawner")
    void StartSpawning();

    UFUNCTION(BlueprintCallable, Category = "Spawner")
    void StopSpawning();

    UFUNCTION(BlueprintPure, Category = "Spawner")
    int32 GetAliveEnemyCount() const { return AliveEnemies.Num(); }


    UPROPERTY(EditDefaultsOnly, Category = "Spawner")
    TSubclassOf<class AEnemyBase> EnemyClass;


protected:
    virtual void BeginPlay() override;

    
    

    UPROPERTY(EditDefaultsOnly, Category = "Spawner")
    float SpawnInterval = 3.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Spawner")
    float SpawnRadius = 1500.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Spawner")
    int32 MaxEnemies = 10;

    UPROPERTY(EditDefaultsOnly, Category = "Spawner")
    bool bAutoStart = true;

private:
    FTimerHandle SpawnTimerHandle;

    UPROPERTY()
    TArray<class AEnemyBase*> AliveEnemies;

    void SpawnEnemy();
    FVector GetRandomSpawnLocation();
    void CleanupDeadEnemies();
};*/


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class ARENA_API AEnemySpawner : public AActor
{
    GENERATED_BODY()

public:
    AEnemySpawner();
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Spawner")
    void StartSpawning();

    UFUNCTION(BlueprintCallable, Category = "Spawner")
    void StopSpawning();

    UFUNCTION(BlueprintPure, Category = "Spawner")
    int32 GetAliveEnemyCount() const { return AliveEnemies.Num(); }

    
    UFUNCTION(BlueprintCallable, Category = "Spawner")
    void SetCurrentWave(int32 NewWave) { CurrentWave = NewWave; }

    UFUNCTION(BlueprintCallable, Category = "Spawner")
    void RegisterEnemy(AEnemyBase* Enemy);

    UFUNCTION(BlueprintPure, Category = "Spawner")
    int32 GetMaxEnemies() const { return MaxEnemies; }

   
    UPROPERTY(EditDefaultsOnly, Category = "Spawner")
    TSubclassOf<class AEnemyBase> EnemyClass;

    
    UPROPERTY(EditDefaultsOnly, Category = "Spawner|Enemy Types")
    TSubclassOf<class AEnemyBase> FastEnemyClass;

    UPROPERTY(EditDefaultsOnly, Category = "Spawner|Enemy Types")
    TSubclassOf<class AEnemyBase> TankEnemyClass;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, Category = "Spawner")
    float SpawnInterval = 3.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Spawner")
    float SpawnRadius = 1500.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Spawner")
    int32 MaxEnemies = 10;

    UPROPERTY(EditDefaultsOnly, Category = "Spawner")
    bool bAutoStart = true;

private:
    FTimerHandle SpawnTimerHandle;

   
    int32 CurrentWave = 1;

    UPROPERTY()
    TArray<class AEnemyBase*> AliveEnemies;

    void SpawnEnemy();
    FVector GetRandomSpawnLocation();
    void CleanupDeadEnemies();

    
    TSubclassOf<AEnemyBase> SelectEnemyClass() const;
};