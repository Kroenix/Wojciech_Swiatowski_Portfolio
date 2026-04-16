#include "DamageNumber.h"
#include "DamageNumberWidget.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"

ADamageNumber::ADamageNumber()
{
    PrimaryActorTick.bCanEverTick = true;

    WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
    RootComponent = WidgetComponent;

   
    WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
    WidgetComponent->SetDrawSize(FVector2D(120.0f, 50.0f));
    WidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ADamageNumber::SetDamageText(float DamageAmount)
{
    UE_LOG(LogTemp, Warning, TEXT("SetDamageText called: %.0f"), DamageAmount);

    if (!DamageWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("✗ DamageWidgetClass is NULL — assign WBP_DamageNumber in BP_DamageNumber"));
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("✓ DamageWidgetClass valid: %s"), *DamageWidgetClass->GetName());

  
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC)
    {
        UE_LOG(LogTemp, Error, TEXT("✗ No PlayerController found"));
        return;
    }

    DamageWidget = CreateWidget<UDamageNumberWidget>(PC, DamageWidgetClass);

    if (!DamageWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("✗ CreateWidget returned NULL"));
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("✓ Widget created successfully"));

    WidgetComponent->SetWidget(DamageWidget);
    DamageWidget->SetDamage(DamageAmount);

    UE_LOG(LogTemp, Warning, TEXT("✓ SetDamageText complete"));
}

void ADamageNumber::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    ElapsedTime += DeltaTime;

    
    AddActorWorldOffset(FVector(0.0f, 0.0f, FloatSpeed * DeltaTime));

    
    float FadeStart = Lifetime * 0.5f;
    if (ElapsedTime >= FadeStart)
    {
        float FadeAlpha = 1.0f - ((ElapsedTime - FadeStart) / (Lifetime - FadeStart));
        FadeAlpha = FMath::Clamp(FadeAlpha, 0.0f, 1.0f);

        if (DamageWidget)
        {
            DamageWidget->SetRenderOpacity(FadeAlpha);
        }
    }

   
    if (ElapsedTime >= Lifetime)
    {
        Destroy();
    }
}