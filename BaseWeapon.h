
#pragma once

#include "CoreMinimal.h"
#include "WeaponType.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

UCLASS()
class ARENA_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseWeapon();
    virtual void Tick(float DeltaTime) override;

   

    UFUNCTION(BlueprintPure, Category = "Weapon")
    virtual bool CanFire() const;



    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void Fire();


    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    FString WeaponName = "Base Weapon";
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    float FireRate = 0.1f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    float Damage = 100.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    int32 ProjectilesPerShot = 1;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    float SpreadAngle = 0.0f;
    UPROPERTY(BlueprintReadOnly, Category = "Weapon")
    int32 MaxAmmo = 100;
    UPROPERTY(BlueprintReadOnly, Category = "Weapon")
    int32 CurrentAmmo = MaxAmmo;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* WeaponMesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USceneComponent* MuzzleLocation;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    TSubclassOf<class AProjectile> ProjectileClass;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    class UParticleSystem* MuzzleFlashEffect;
    UPROPERTY(EditDefaultsOnly, Category = "Sound")
    TArray<class USoundBase*> FireSounds;


    UFUNCTION(BlueprintPure, Category = "Weapon")
    FString GetWeaponName() const { return WeaponName; }
    UFUNCTION(BlueprintPure, Category = "Weapon")
    int32 GetCurrentAmmo() const { return CurrentAmmo; }
    UFUNCTION(BlueprintPure, Category = "Weapon")
    int32 GetMaxAmmo() const { return MaxAmmo; }
     UFUNCTION(BlueprintCallable, Category = "Weapon")
    void AddAmmo(int32 Amount);
    UFUNCTION(BlueprintPure, Category = "Weapon")
    EWeaponType GetWeaponType() const { return WeaponAnimationType; }


protected:
	virtual void BeginPlay() override;

    
    void SpawnProjectile(const FVector& Direction);



    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    bool bInfiniteAmmo = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Animation")
    EWeaponType WeaponAnimationType = EWeaponType::Pistol;

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    TSubclassOf<class UCameraShakeBase> FireCameraShake;



private:
    bool bCanFire = true;
    FTimerHandle FireRateTimerHandle;
    float LastFireTime = 0.0f;
};
