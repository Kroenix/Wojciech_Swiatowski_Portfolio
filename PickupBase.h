#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupBase.generated.h"

UCLASS()
class ARENA_API APickupBase : public AActor
{
    GENERATED_BODY()

public:
    APickupBase();
    virtual void Tick(float DeltaTime) override;

protected:
    virtual void BeginPlay() override;

  
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* PickupMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USphereComponent* CollisionSphere;

    
    UPROPERTY(EditDefaultsOnly, Category = "Pickup")
    FLinearColor PickupColor = FLinearColor::White;

    UPROPERTY(EditDefaultsOnly, Category = "Pickup")
    bool bRotate = true;

    UPROPERTY(EditDefaultsOnly, Category = "Pickup")
    float RotationSpeed = 90.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Pickup")
    bool bBobUpDown = true;

    UPROPERTY(EditDefaultsOnly, Category = "Pickup")
    float BobSpeed = 2.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Pickup")
    float BobAmount = 20.0f;

   
    UPROPERTY(EditDefaultsOnly, Category = "Sound")
    class USoundBase* PickupSound;

   
    UPROPERTY()
    class UMaterialInstanceDynamic* DynamicMaterial;

    
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
        const FHitResult& SweepResult);

    
    virtual void OnPickedUp(class AMyArenaCharacter* Character);

private:
    FVector StartLocation;
    float BobTime = 0.0f;
};