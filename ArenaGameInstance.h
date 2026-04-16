
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ArenaGameInstance.generated.h"


UCLASS()
class ARENA_API UArenaGameInstance : public UGameInstance
{
	GENERATED_BODY()
	

public:
    UArenaGameInstance();


   


    
    UFUNCTION(BlueprintCallable, Category = "Level")
    void LoadLevelWithFade(FName LevelName, float FadeDuration = 4.0f);
    UFUNCTION(BlueprintPure, Category = "UI")
    class UFadeWidget* GetFadeWidget() const { return FadeWidget; }
    UFUNCTION(BlueprintCallable, Category = "Fade")
    void StartFadeOut(float Duration);

    UFUNCTION(BlueprintCallable, Category = "Fade")
    void StartFadeIn(float Duration);
    UFUNCTION(BlueprintCallable, Category = "UI")
    void RemoveFadeWidget();

protected:
   
    UPROPERTY()
    class UFadeWidget* FadeWidget;

    void CreateFadeWidget();

private:
    FName PendingLevelName;
    float PendingFadeDuration;
    FTimerHandle LevelLoadTimer;


    void ActuallyLoadLevel();
};
