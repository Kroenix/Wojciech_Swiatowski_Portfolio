#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageNumber.generated.h"

UCLASS()
class ARENA_API ADamageNumber : public AActor
{
	GENERATED_BODY()
	
public:
    ADamageNumber();
    virtual void Tick(float DeltaTime) override;


    void SetDamageText(float DamageAmount);

    
    UPROPERTY(EditDefaultsOnly, Category = "Damage")
    TSubclassOf<class UDamageNumberWidget> DamageWidgetClass;

private:
    UPROPERTY(VisibleAnywhere)
    class UWidgetComponent* WidgetComponent;

    
    UPROPERTY(EditDefaultsOnly, Category = "Damage")
    float Lifetime = 1.2f;

    
    UPROPERTY(EditDefaultsOnly, Category = "Damage")
    float FloatSpeed = 80.0f;

    float ElapsedTime = 0.0f;

    UPROPERTY()
    class UDamageNumberWidget* DamageWidget;
};