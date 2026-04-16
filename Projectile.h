
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class ARENA_API AProjectile : public AActor
{
	GENERATED_BODY()

	
public:	
	AProjectile();


	virtual void Tick(float DeltaTime) override;


	void FireInDirection(const FVector& ShootDirection);


	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float Damage = 10.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Projectile")
	float ProjectileSpeed = 3000.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Projectile")
	float LifeTime = 2.0f;


	void SetDamage(float NewDamage) { Damage = NewDamage; }
	float GetDamage() const { return Damage; }


protected:


	virtual void BeginPlay() override;


	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,FVector NormalImpulse, const FHitResult& Hit);


private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USphereComponent* CollisionComponent;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UStaticMeshComponent* ProjectileMeshComponent;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovementComponent;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UParticleSystemComponent* TrailEffect;


	


};
