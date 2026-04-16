
#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ArenaGameInstance.h"

void UMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    
    StartButton = Cast<UButton>(GetWidgetFromName(TEXT("StartButton")));
    QuitButton = Cast<UButton>(GetWidgetFromName(TEXT("QuitButton")));
    TitleText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TitleText")));

    
    UE_LOG(LogTemp, Warning, TEXT("=== MainMenu Widget Debug ==="));
    UE_LOG(LogTemp, Warning, TEXT("StartButton valid? %s"), StartButton ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("QuitButton valid? %s"), QuitButton ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("TitleText valid? %s"), TitleText ? TEXT("YES") : TEXT("NO"));

    if (StartButton)
    {
        StartButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnStartClicked);
    }

    if (QuitButton)
    {
        QuitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitClicked);
    }

    if (TitleText)
    {
        TitleText->SetText(FText::FromString(TEXT("ARENA SHOOTER")));
    }
}

void UMainMenuWidget::OnStartClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("START BUTTON CLICKED"));

    APlayerController* PC = GetOwningPlayer();
    if (!PC)
    {
        UGameplayStatics::OpenLevel(this, FName("ArenaMap"));
        return;
    }

    
    this->SetVisibility(ESlateVisibility::Hidden);
    UE_LOG(LogTemp, Warning, TEXT("Menu hidden"));

   
    if (PC->PlayerCameraManager)
    {
        PC->PlayerCameraManager->StartCameraFade(
            0.0f,  
            1.0f,  
            1.5f,  
            FLinearColor::Black,
            false,
            true   

        UE_LOG(LogTemp, Warning, TEXT("Fading to black..."));
    }

   
    FTimerHandle LoadTimer;
    GetWorld()->GetTimerManager().SetTimer(LoadTimer, [this]()
        {
            UE_LOG(LogTemp, Warning, TEXT("Loading level..."));
            UGameplayStatics::OpenLevel(this, FName("ArenaMap"));

        }, 2.2f, false); 
}

void UMainMenuWidget::OnQuitClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Quit Button Clicked!"));

    
    APlayerController* PC = GetOwningPlayer();


    if (PC)
    {
        UKismetSystemLibrary::QuitGame(GetWorld(), PC, EQuitPreference::Quit, false);
    }
}

void UMainMenuWidget::LoadGameLevel()
{
    UE_LOG(LogTemp, Warning, TEXT("Loading game level..."));

    
    UArenaGameInstance* GameInstance = Cast<UArenaGameInstance>(GetGameInstance());
    if (GameInstance)
    {
       
        GameInstance->LoadLevelWithFade(FName("ArenaMap"), 1.0f);
    }
    else
    {
       
        UGameplayStatics::OpenLevel(this, FName("ArenaMap"));
    }
}