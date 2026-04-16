
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameHUD.generated.h"


UCLASS()
class ARENA_API UGameHUD : public UUserWidget
{
	GENERATED_BODY()
	

public:
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateHealth(float CurrentHealth, float MaxHealth);
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateAmmo(const FString& WeaponName, int32 CurrentAmmo, int32 MaxAmmo);
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateScore(int32 NewScore);
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateWeaponName(const FString& WeaponName);
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateWave(int32 CurrentWave);
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowAnnouncement(const FString& Message, float Duration, bool bLarge = true);
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowCountdown(int32 Seconds);
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateEnemiesRemaining(int32 Remaining, int32 Total);
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateWaveProgress(int32 Killed, int32 Remaining, int32 Total);
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowBossHealthBar(const FString& BossName);
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateBossHealth(float CurrentHealth, float MaxHealth);
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void HideBossHealthBar();


protected:
    virtual void NativeConstruct() override;

   
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UProgressBar* HealthBar;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UTextBlock* HealthText;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UTextBlock* AmmoText;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UTextBlock* ScoreText;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UTextBlock* WeaponNameText;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UTextBlock* WaveText;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UTextBlock* AnnouncementText;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UTextBlock* EnemiesRemainingText;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UProgressBar* BossHealthBar;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UTextBlock* BossNameText;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UTextBlock* BossHealthText;
    

private:
    float CurrentHealthPercent = 1.0f;
    float CurrentBossHealthPercent = 1.0f;
    float TargetBossHealthPercent = 1.0f;
        

};