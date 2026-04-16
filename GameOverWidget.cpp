#include "GameOverWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ArenaGameInstance.h"

void UGameOverWidget::NativeConstruct()
{
    Super::NativeConstruct();

    UE_LOG(LogTemp, Warning, TEXT("=== Game Over Widget Constructed ==="));

   
    if (RestartButton)
    {
        RestartButton->OnClicked.AddDynamic(this, &UGameOverWidget::OnRestartClicked);
        UE_LOG(LogTemp, Log, TEXT("✓ Restart button bound"));
    }

    if (MainMenuButton)
    {
        MainMenuButton->OnClicked.AddDynamic(this, &UGameOverWidget::OnMainMenuClicked);
        UE_LOG(LogTemp, Log, TEXT("✓ Main Menu button bound"));
    }

    if (QuitButton)
    {
        QuitButton->OnClicked.AddDynamic(this, &UGameOverWidget::OnQuitClicked);
        UE_LOG(LogTemp, Log, TEXT("✓ Quit button bound"));
    }

  
    if (GameOverTitle)
    {
        GameOverTitle->SetText(FText::FromString(TEXT("GAME OVER")));
    }
}

void UGameOverWidget::ShowGameOver(int32 FinalScore, int32 EnemiesKilled, float SurvivalTime)
{
    UE_LOG(LogTemp, Warning, TEXT("=== SHOWING GAME OVER ==="));
    UE_LOG(LogTemp, Warning, TEXT("Final Score: %d"), FinalScore);
    UE_LOG(LogTemp, Warning, TEXT("Enemies Killed: %d"), EnemiesKilled);
    UE_LOG(LogTemp, Warning, TEXT("Survival Time: %.1f seconds"), SurvivalTime);

  
    if (FinalScoreText)
    {
        FString ScoreString = FString::Printf(TEXT("FINAL SCORE: %d"), FinalScore);
        FinalScoreText->SetText(FText::FromString(ScoreString));
    }

   
    if (EnemiesKilledText)
    {
        FString KillsString = FString::Printf(TEXT("Enemies Killed: %d"), EnemiesKilled);
        EnemiesKilledText->SetText(FText::FromString(KillsString));
    }

  
    if (SurvivalTimeText)
    {
        int32 Minutes = FMath::FloorToInt(SurvivalTime / 60.0f);
        int32 Seconds = FMath::FloorToInt(SurvivalTime) % 60;
        FString TimeString = FString::Printf(TEXT("Survival Time: %02d:%02d"), Minutes, Seconds);
        SurvivalTimeText->SetText(FText::FromString(TimeString));
    }

  
    int32 HighScore = LoadHighScore();

    if (FinalScore > HighScore)
    {
        UE_LOG(LogTemp, Warning, TEXT("🏆 NEW HIGH SCORE! 🏆"));
        SaveHighScore(FinalScore);
        HighScore = FinalScore;

        if (HighScoreText)
        {
            HighScoreText->SetText(FText::FromString(TEXT("🏆 NEW HIGH SCORE! 🏆")));
            HighScoreText->SetColorAndOpacity(FLinearColor::Yellow);
        }
    }
    else
    {
        if (HighScoreText)
        {
            FString HighScoreString = FString::Printf(TEXT("High Score: %d"), HighScore);
            HighScoreText->SetText(FText::FromString(HighScoreString));
            HighScoreText->SetColorAndOpacity(FLinearColor::White);
        }
    }
}

void UGameOverWidget::OnRestartClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Restart clicked - Reloading level..."));

  
    FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());

  
    UArenaGameInstance* GameInstance = Cast<UArenaGameInstance>(GetGameInstance());
    if (GameInstance)
    {
        GameInstance->LoadLevelWithFade(FName(*CurrentLevelName), 0.5f);
    }
    else
    {
      
        UGameplayStatics::OpenLevel(this, FName(*CurrentLevelName));
    }
}

void UGameOverWidget::OnMainMenuClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Main Menu clicked - Returning to menu..."));

    
    UArenaGameInstance* GameInstance = Cast<UArenaGameInstance>(GetGameInstance());
    if (GameInstance)
    {
        GameInstance->LoadLevelWithFade(FName("MainMenu"), 0.5f);
    }
    else
    {
        UGameplayStatics::OpenLevel(this, FName("MainMenu"));
    }
}

void UGameOverWidget::OnQuitClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("Quit clicked - Exiting game..."));

    APlayerController* PC = GetOwningPlayer();
    if (PC)
    {
        UKismetSystemLibrary::QuitGame(GetWorld(), PC, EQuitPreference::Quit, false);
    }
}

void UGameOverWidget::SaveHighScore(int32 Score)
{
    
    UArenaGameInstance* GameInstance = Cast<UArenaGameInstance>(GetGameInstance());
    if (GameInstance)
    {
        
    }

    
    if (GConfig)
    {
        GConfig->SetInt(
            TEXT("GameStats"),
            TEXT("HighScore"),
            Score,
            GGameUserSettingsIni
        );
        GConfig->Flush(false, GGameUserSettingsIni);

        UE_LOG(LogTemp, Log, TEXT("✓ High score saved: %d"), Score);
    }
}

int32 UGameOverWidget::LoadHighScore()
{
    int32 HighScore = 0;

    if (GConfig)
    {
        GConfig->GetInt(
            TEXT("GameStats"),
            TEXT("HighScore"),
            HighScore,
            GGameUserSettingsIni
        );
    }

    UE_LOG(LogTemp, Log, TEXT("Loaded high score: %d"), HighScore);
    return HighScore;
}