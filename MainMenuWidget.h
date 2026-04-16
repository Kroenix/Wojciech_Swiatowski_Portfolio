
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

UCLASS()
class ARENA_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	

protected:
    virtual void NativeConstruct() override;

 
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* StartButton;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* QuitButton;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UTextBlock* TitleText;

private:
    UFUNCTION()
    void OnStartClicked();
    UFUNCTION()
    void OnQuitClicked();

   
    void LoadGameLevel();

};
