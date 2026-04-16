
#include "FadeWidget.h"
#include "Components/Image.h"

void UFadeWidget::FadeToBlack(float Duration)
{
    UE_LOG(LogTemp, Warning, TEXT("FadeWidget: Fading TO black over %.1f seconds"), Duration);

    if (!FadeImage)
    {
        UE_LOG(LogTemp, Error, TEXT("FadeImage is NULL!"));
        return;
    }

  
    float StartOpacity = FadeImage->GetRenderOpacity();
    float TargetOpacity = 1.0f;

    UE_LOG(LogTemp, Log, TEXT("Starting opacity: %.2f → Target: %.2f"), StartOpacity, TargetOpacity);

   
    if (FMath::IsNearlyEqual(StartOpacity, TargetOpacity, 0.01f))
    {
        FadeImage->SetRenderOpacity(TargetOpacity);
        UE_LOG(LogTemp, Warning, TEXT("Already at target opacity, set immediately"));
        return;
    }

    
    if (GetWorld()->GetTimerManager().IsTimerActive(FadeTimerHandle))
    {
        GetWorld()->GetTimerManager().ClearTimer(FadeTimerHandle);
    }

    float ElapsedTime = 0.0f;
    const float TickRate = 0.016f; 

    GetWorld()->GetTimerManager().SetTimer(FadeTimerHandle, [this, StartOpacity, TargetOpacity, Duration, ElapsedTime]() mutable
        {
            ElapsedTime += TickRate;
            float Alpha = FMath::Clamp(ElapsedTime / Duration, 0.0f, 1.0f);

            
            float CurrentOpacity = FMath::Lerp(StartOpacity, TargetOpacity, Alpha);

            if (FadeImage)
            {
                FadeImage->SetRenderOpacity(CurrentOpacity);
            }

           
            if (Alpha >= 1.0f)
            {
                GetWorld()->GetTimerManager().ClearTimer(FadeTimerHandle);
                UE_LOG(LogTemp, Warning, TEXT("✓ Fade TO black complete (Opacity: %.2f)"), CurrentOpacity);
            }

        }, TickRate, true);
}

void UFadeWidget::FadeFromBlack(float Duration)
{
    UE_LOG(LogTemp, Warning, TEXT("FadeWidget: Fading FROM black over %.1f seconds"), Duration);

    if (!FadeImage)
    {
        UE_LOG(LogTemp, Error, TEXT("FadeImage is NULL!"));
        return;
    }

    
    float StartOpacity = FadeImage->GetRenderOpacity();
    float TargetOpacity = 0.0f;

    UE_LOG(LogTemp, Log, TEXT("Starting opacity: %.2f → Target: %.2f"), StartOpacity, TargetOpacity);

    
    if (FMath::IsNearlyEqual(StartOpacity, TargetOpacity, 0.01f))
    {
        FadeImage->SetRenderOpacity(TargetOpacity);
        UE_LOG(LogTemp, Warning, TEXT("Already at target opacity, set immediately"));
        return;
    }

    
    if (GetWorld()->GetTimerManager().IsTimerActive(FadeTimerHandle))
    {
        GetWorld()->GetTimerManager().ClearTimer(FadeTimerHandle);
    }

    float ElapsedTime = 0.0f;
    const float TickRate = 0.016f;

    GetWorld()->GetTimerManager().SetTimer(FadeTimerHandle, [this, StartOpacity, TargetOpacity, Duration, ElapsedTime]() mutable
        {
            ElapsedTime += TickRate;
            float Alpha = FMath::Clamp(ElapsedTime / Duration, 0.0f, 1.0f);

            
            float CurrentOpacity = FMath::Lerp(StartOpacity, TargetOpacity, Alpha);

            if (FadeImage)
            {
                FadeImage->SetRenderOpacity(CurrentOpacity);
            }

           
            if (Alpha >= 1.0f)
            {
                GetWorld()->GetTimerManager().ClearTimer(FadeTimerHandle);
                UE_LOG(LogTemp, Warning, TEXT("✓ Fade FROM black complete (Opacity: %.2f)"), CurrentOpacity);
            }

        }, TickRate, true);
}

void UFadeWidget::SetBlockInput(bool bBlock)
{
    if (bBlock)
    {
        SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        
        SetVisibility(ESlateVisibility::HitTestInvisible);
    }

    UE_LOG(LogTemp, Log, TEXT("Fade widget input blocking: %s"), bBlock ? TEXT("ON") : TEXT("OFF"));
}

void UFadeWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (FadeImage)
    {
        FadeImage->SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f));
        UE_LOG(LogTemp, Log, TEXT("✓ FadeImage initialized"));
    }

    
    SetBlockInput(false); 
}

void UFadeWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);


    if (bIsFading)
    {
        UpdateFade(InDeltaTime);
    }
}

void UFadeWidget::UpdateFade(float DeltaTime)
{
    CurrentFadeTime += DeltaTime;
    float Alpha = FMath::Clamp(CurrentFadeTime / FadeDuration, 0.0f, 1.0f);

    if (bFadingToBlack)
    {
        CurrentAlpha = Alpha;
    }
    else
    {
        CurrentAlpha = 1.0f - Alpha;
    }

    if (FadeImage)
    {
        FadeImage->SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, CurrentAlpha));
    }

    if (Alpha >= 1.0f)
    {
        bIsFading = false;
        SetBlockInput(false); // Unblock input when fade completes
        UE_LOG(LogTemp, Warning, TEXT("=== FADE COMPLETE ==="));
    }
}
