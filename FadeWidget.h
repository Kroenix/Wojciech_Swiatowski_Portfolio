
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FadeWidget.generated.h"

UCLASS()
class ARENA_API UFadeWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	UFUNCTION(Blueprintcallable, Category = "Fade")
	void FadeToBlack(float Duration = 4.0f);
	UFUNCTION(Blueprintcallable, Category = "Fade")
	void FadeFromBlack(float Duration = 4.0f);
	UFUNCTION(Blueprintcallable, Category = "Fade")
	bool IsFading() const { return bIsFading; }

	UFUNCTION(BlueprintPure, Category = "Fade")
	class UImage* GetFadeImage() const { return FadeImage; }
	


	void SetBlockInput(bool bBlock);


protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


private:
	UPROPERTY(meta = (BindWidget))
	class UImage* FadeImage;

	bool bIsFading = false;
	bool bFadingToBlack = false;
	float FadeDuration = 2.0f;
	float CurrentFadeTime = 0.0f;
	float CurrentAlpha = 0.0f;


	FTimerHandle FadeTimerHandle;


	void UpdateFade(float DeltaTime);
};
