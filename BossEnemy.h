// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "BossEnemy.generated.h"

UENUM(BlueprintType)
enum class EBossAbility : uint8
{
    None,
    SpawnMinions,
    AOEAttack,
    ChargeAttack,
    Shield,
    Enrage
};

UCLASS()
class ARENA_API ABossEnemy : public AEnemyBase
{
	GENERATED_BODY()
	

public:
    ABossEnemy();

protected:
    virtual void BeginPlay() override;
    FLinearColor OriginalBossColor;
    void FlashDamageEffect();


public:
    virtual void Tick(float DeltaTime) override;
    virtual void TakeDamage(float DamageAmount) override;
    virtual void Die() override;

   
    UPROPERTY(EditDefaultsOnly, Category = "Boss")
    float HealthMultiplier = 10.0f;
    UPROPERTY(EditDefaultsOnly, Category = "Boss")
    float SizeMultiplier = 2.0f;
    UPROPERTY(EditDefaultsOnly, Category = "Boss")
    int32 BonusScoreMultiplier = 5;
    UPROPERTY(EditDefaultsOnly, Category = "Boss")
    FLinearColor BossColor = FLinearColor(0.8f, 0.0f, 0.8f, 1.0f); 
    UPROPERTY(EditDefaultsOnly, Category = "Boss")
    float SpeedMultiplier = 1.2f;


    UPROPERTY(EditDefaultsOnly, Category = "Boss Abilities")
    bool bCanSpawnMinions = true;
    UPROPERTY(EditDefaultsOnly, Category = "Boss Abilities")
    bool bCanUseAOE = true;
    UPROPERTY(EditDefaultsOnly, Category = "Boss Abilities")
    bool bCanCharge = true;
    UPROPERTY(EditDefaultsOnly, Category = "Boss Abilities")
    bool bCanUseShield = true;
    UPROPERTY(EditDefaultsOnly, Category = "Boss Abilities")
    float AbilityCooldown = 10.0f;
    UPROPERTY(EditDefaultsOnly, Category = "Boss Abilities")
    float EnrageHealthPercent = 0.25f; 
    UPROPERTY(EditDefaultsOnly, Category = "Boss Abilities")
    TSubclassOf<class AEnemyBase> MinionClass;


protected:
    
    FTimerHandle AbilityTimerHandle;
    EBossAbility CurrentAbility = EBossAbility::None;
    bool bIsEnraged = false;
    bool bIsShielded = false;
    bool bIsCharging = false;

    float LastAbilityTime = 0.0f;
    FVector ChargeStartLocation;
    FVector ChargeTargetLocation;
    float ChargeProgress = 0.0f;

   
    void TryUseAbility();
    void UseRandomAbility();

    void SpawnMinions();
    void UseAOEAttack();
    void StartChargeAttack();
    void ActivateShield();
    void EnterEnrageMode();

    void UpdateChargeAttack(float DeltaTime);
    void UpdateShield(float DeltaTime);

    bool CanUseAbility() const;

   
    class AMyArenaCharacter* GetPlayerCharacter();

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    TSubclassOf<class UCameraShakeBase> AbilityCameraShake;


};
