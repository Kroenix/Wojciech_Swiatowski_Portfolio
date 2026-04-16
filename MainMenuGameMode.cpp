
#include "MainMenuGameMode.h"
#include "MainMenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"


AMainMenuGameMode::AMainMenuGameMode()
{
    DefaultPawnClass = nullptr;
}

void AMainMenuGameMode::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Warning, TEXT("Main Menu Game Mode - BeginPlay"));

    
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC)
    {
        UE_LOG(LogTemp, Error, TEXT("No PlayerController!"));
        return;
    }

    
    if (MainMenuWidgetClass)
    {
        MainMenuWidget = CreateWidget<UMainMenuWidget>(PC, MainMenuWidgetClass);
        if (MainMenuWidget)
        {
            MainMenuWidget->AddToViewport();
            UE_LOG(LogTemp, Log, TEXT("✓ Main Menu Widget displayed"));
        }
    }

  
    FInputModeUIOnly InputMode;
    InputMode.SetWidgetToFocus(MainMenuWidget->TakeWidget());
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

    PC->SetInputMode(InputMode);
    PC->bShowMouseCursor = true;
    PC->bEnableClickEvents = true;
    PC->bEnableMouseOverEvents = true;

    UE_LOG(LogTemp, Warning, TEXT("✓ Input mode set to UI only"));
    UE_LOG(LogTemp, Warning, TEXT("✓ Mouse cursor enabled"));
}