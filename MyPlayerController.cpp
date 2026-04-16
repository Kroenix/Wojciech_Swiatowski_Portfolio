
#include "MyPlayerController.h"
#include "MyGameMode.h"

AMyPlayerController::AMyPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void AMyPlayerController::BeginPlay()
{
    Super::BeginPlay();

    
    AMyGameMode* GameMode = Cast<AMyGameMode>(GetWorld()->GetAuthGameMode());
    if (GameMode)
    {
        
        UE_LOG(LogTemp, Warning, TEXT("Game Level - Setting Game Input Mode"));

        FInputModeGameAndUI InputMode;
        InputMode.SetHideCursorDuringCapture(false);
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        SetInputMode(InputMode);

        bShowMouseCursor = true;
        bEnableClickEvents = true;
        bEnableMouseOverEvents = true;

        UE_LOG(LogTemp, Log, TEXT("? Game input mode enabled"));
    }
    else
    {
        
        UE_LOG(LogTemp, Log, TEXT("Not in game level (probably main menu)"));
    }
}
