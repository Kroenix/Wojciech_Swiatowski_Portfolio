#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverWidget.generated.h"

UCLASS()
class ARENA_API UGameOverWidget : public UUserWidget
{
    GENERATED_BODY()

public:
 
    UFUNCTION(BlueprintCallable, Category = "GameOver")
    void ShowGameOver(int32 FinalScore, int32 EnemiesKilled, float SurvivalTime);

protected:
    virtual void NativeConstruct() override;

   
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UTextBlock* GameOverTitle;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UTextBlock* FinalScoreText;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UTextBlock* EnemiesKilledText;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UTextBlock* SurvivalTimeText;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UTextBlock* HighScoreText;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* RestartButton;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* MainMenuButton;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* QuitButton;

private:
    UFUNCTION()
    void OnRestartClicked();

    UFUNCTION()
    void OnMainMenuClicked();

    UFUNCTION()
    void OnQuitClicked();

    void SaveHighScore(int32 Score);
    int32 LoadHighScore();
};