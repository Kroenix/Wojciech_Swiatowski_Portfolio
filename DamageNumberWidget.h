#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageNumberWidget.generated.h"

UCLASS()
class ARENA_API UDamageNumberWidget : public UUserWidget
{
    GENERATED_BODY()

public:
  
    UFUNCTION(BlueprintCallable, Category = "Damage")
    void SetDamage(float DamageAmount);


    UPROPERTY(BlueprintReadWrite, Category = "Damage")
    float Opacity = 1.0f;

protected:
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UTextBlock* DamageText;
};