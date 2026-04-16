#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"


UCLASS()
class ARENA_API AEnemyBase : public ACharacter
{
    GENERATED_BODY()


public:
    AEnemyBase();
    virtual void Tick(float DeltaTime) override;

    
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual void TakeDamage(float DamageAmount);

    UFUNCTION(BlueprintPure, Category = "Combat")
    bool IsDead() const { return bIsDead; }

    UFUNCTION(BlueprintPure, Category = "Combat")
    float GetHealthPercent() const { return CurrentHealth / MaxHealth; }

    UFUNCTION(BlueprintCallable, Category = "Health")
    virtual  void Die();
   

protected:
    virtual void BeginPlay() override;


    bool bIsDead = false;

    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* BodyMesh;

    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    float MaxHealth = 50.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    float CurrentHealth = 50.0f;

    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
    float MoveSpeed = 300.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
    float ChaseRange = 2000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
    float AttackRange = 150.0f;

   
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    float AttackDamage = 10.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    float AttackCooldown = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    int32 ScoreValue = 100;

    
    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    class UParticleSystem* DeathEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Sound")
    class USoundBase* HitSound;

    UPROPERTY(EditDefaultsOnly, Category = "Sound")
    class USoundBase* DeathSound;

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    FLinearColor EnemyColor = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    TSubclassOf<class ADamageNumber> DamageNumberClass;


    UPROPERTY()
    class UMaterialInstanceDynamic* BodyMaterialInstance;


    UPROPERTY(EditDefaultsOnly, Category = "Drops")
    float HealthDropChance = 0.10f; // 10%
    UPROPERTY(EditDefaultsOnly, Category = "Drops")
    float AmmoDropChance = 0.15f; // 15%
    UPROPERTY(EditDefaultsOnly, Category = "Drops")
    TSubclassOf<class AHealthPickup> HealthPickupClass;
    UPROPERTY(EditDefaultsOnly, Category = "Drops")
    TSubclassOf<class AAmmoPickup> AmmoPickupClass;
    void TryDropPickup();

    void SpawnDamageNumber(float DamageAmount);


private:

   
    
    float LastAttackTime = 0.0f;

    UPROPERTY()
    class AMyArenaCharacter* PlayerTarget;

    

    void MoveTowardsPlayer(float DeltaTime);
    void TryAttackPlayer();
    
    void FlashDamageColor();
};
