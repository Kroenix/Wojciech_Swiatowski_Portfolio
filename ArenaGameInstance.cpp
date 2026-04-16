
#include "ArenaGameInstance.h"
#include "FadeWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Image.h"
#include "TimerManager.h"

UArenaGameInstance::UArenaGameInstance()
{
}


void UArenaGameInstance::LoadLevelWithFade(FName LevelName, float FadeDuration)
{
    UE_LOG(LogTemp, Warning, TEXT("=== Loading Level with Fade ==="));

    UWorld* World = GetWorld();
    if (!World)
    {
        UGameplayStatics::OpenLevel(this, LevelName);
        return;
    }

    
    if (!FadeWidget)
    {
        CreateFadeWidget();
    }

    if (!FadeWidget || !FadeWidget->GetFadeImage())
    {
        UE_LOG(LogTemp, Error, TEXT("No fade widget, loading directly"));
        UGameplayStatics::OpenLevel(World, LevelName);
        return;
    }

    
    UE_LOG(LogTemp, Warning, TEXT("Setting screen to BLACK"));
    FadeWidget->SetVisibility(ESlateVisibility::Visible);
    FadeWidget->GetFadeImage()->SetRenderOpacity(1.0f);
    FadeWidget->GetFadeImage()->SetColorAndOpacity(FLinearColor::Black);

   
    FTimerHandle LoadTimer;
    World->GetTimerManager().SetTimer(LoadTimer, [World, LevelName]()
        {
            UE_LOG(LogTemp, Warning, TEXT("Loading level while black screen is showing..."));
            UGameplayStatics::OpenLevel(World, LevelName);

        }, 0.5f, false); 
}

void UArenaGameInstance::StartFadeOut(float Duration)
{
    UE_LOG(LogTemp, Warning, TEXT("StartFadeOut called (Duration: %.1f)"), Duration);

    if (!FadeWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("FadeWidget is NULL, creating it..."));
        CreateFadeWidget();
    }

    if (FadeWidget)
    {
        FadeWidget->FadeToBlack(Duration);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("✗ Failed to create FadeWidget!"));
    }
}

void UArenaGameInstance::StartFadeIn(float Duration)
{
    UE_LOG(LogTemp, Warning, TEXT("StartFadeIn called (Duration: %.1f)"), Duration);

    if (!FadeWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("FadeWidget is NULL, creating it..."));
        CreateFadeWidget();
    }

    if (FadeWidget)
    {
        FadeWidget->FadeFromBlack(Duration);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("✗ Failed to create FadeWidget!"));
    }
}

void UArenaGameInstance::RemoveFadeWidget()
{
    if (FadeWidget)
    {
        FadeWidget->RemoveFromParent();
        FadeWidget = nullptr;
        UE_LOG(LogTemp, Log, TEXT("✓ Fade widget removed"));
    }
}



void UArenaGameInstance::ActuallyLoadLevel()
{
    UE_LOG(LogTemp, Warning, TEXT("Actually loading level: %s"), *PendingLevelName.ToString());

   
        FadeWidget = nullptr;
   

    UGameplayStatics::OpenLevel(this, PendingLevelName);;
}


void UArenaGameInstance::CreateFadeWidget()
{
    UE_LOG(LogTemp, Error, TEXT("========================================"));
    UE_LOG(LogTemp, Error, TEXT("  CREATING FADE WIDGET"));
    UE_LOG(LogTemp, Error, TEXT("========================================"));

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("✗ World is NULL"));
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("✓ World is valid"));

    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC)
    {
        UE_LOG(LogTemp, Error, TEXT("✗ PlayerController is NULL"));
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("✓ PlayerController is valid"));

    
    TSubclassOf<UFadeWidget> LoadedFadeWidgetClass = LoadClass<UFadeWidget>(nullptr,
        TEXT("/Game/UI/WBP_Fade.WBP_Fade_C"));

    if (!LoadedFadeWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("✗ Failed to load WBP_Fade"));
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("✓ WBP_Fade class loaded"));

   
    FadeWidget = CreateWidget<UFadeWidget>(PC, LoadedFadeWidgetClass);
    if (!FadeWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("✗ Failed to create Fade Widget instance"));
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("✓ Fade Widget instance created"));

   
    FadeWidget->AddToViewport(10000);
    UE_LOG(LogTemp, Warning, TEXT("✓ Added to viewport"));

    
    FadeWidget->SetVisibility(ESlateVisibility::Visible);

   
    if (FadeWidget->GetFadeImage())
    {
        FadeWidget->GetFadeImage()->SetRenderOpacity(0.0f);
        UE_LOG(LogTemp, Warning, TEXT("✓ FadeImage opacity set to 0.0 (transparent)"));
    }

    UE_LOG(LogTemp, Error, TEXT("========================================"));
}
