
#include "GameHUD.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "TimerManager.h"





void UGameHUD::UpdateHealth(float CurrentHealth, float MaxHealth)
{
    if (HealthText)
    {
        FString HealthString = FString::Printf(TEXT("HP: %.0f / %.0f"), CurrentHealth, MaxHealth);
        HealthText->SetText(FText::FromString(HealthString));
    }

    
    if (HealthBar)
    {
        float HealthPercent = FMath::Clamp(CurrentHealth / MaxHealth, 0.0f, 1.0f);
        HealthBar->SetPercent(HealthPercent);

        
        FLinearColor BarColor;
        if (HealthPercent > 0.6f)
        {
            BarColor = FLinearColor::Green;
        }
        else if (HealthPercent > 0.3f)
        {
            BarColor = FLinearColor::Yellow;
        }
        else
        {
            BarColor = FLinearColor::Red;
        }

        HealthBar->SetFillColorAndOpacity(BarColor);

        UE_LOG(LogTemp, Log, TEXT("Player Health Updated: %.0f/%.0f (%.1f%%)"),
            CurrentHealth, MaxHealth, HealthPercent * 100.0f);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("HealthBar widget is NULL!"));
    }
}

void UGameHUD::UpdateAmmo(const FString& WeaponName, int32 CurrentAmmo, int32 MaxAmmo)
{
    if (WeaponNameText)
    {
        WeaponNameText->SetText(FText::FromString(WeaponName));
    }

    if (AmmoText)
    {
        FString AmmoString = FString::Printf(TEXT("Ammo: %d / %d"), CurrentAmmo, MaxAmmo);
        AmmoText->SetText(FText::FromString(AmmoString));

        
        if (CurrentAmmo == 0)
        {
            AmmoText->SetColorAndOpacity(FLinearColor::Red);
        }
        else if (CurrentAmmo <= MaxAmmo * 0.25f)
        {
            AmmoText->SetColorAndOpacity(FLinearColor::Yellow);
        }
        else
        {
            AmmoText->SetColorAndOpacity(FLinearColor::White);
        }
    }
}

void UGameHUD::UpdateScore(int32 NewScore)
{
    if (ScoreText)
    {
        FString ScoreString = FString::Printf(TEXT("SCORE: %d"), NewScore);
        ScoreText->SetText(FText::FromString(ScoreString));
    }
}



void UGameHUD::UpdateWeaponName(const FString& WeaponName)
{
    if (WeaponNameText)
    {
        WeaponNameText->SetText(FText::FromString(WeaponName));
    }
}

void UGameHUD::UpdateWave(int32 CurrentWave)
{
    if (WaveText)
    {
        FString WaveString = FString::Printf(TEXT("WAVE %d"), CurrentWave);
        WaveText->SetText(FText::FromString(WaveString));
        UE_LOG(LogTemp, Log, TEXT("HUD updated: Wave %d"), CurrentWave);
    }
}

void UGameHUD::ShowAnnouncement(const FString& Message, float Duration, bool bLarge)
{
    if (!AnnouncementText)
    {
        UE_LOG(LogTemp, Warning, TEXT("AnnouncementText not bound!"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT(">>> ANNOUNCEMENT: %s <<<"), *Message);

    
    AnnouncementText->SetText(FText::FromString(Message));

    if (bLarge)
    {
        
        AnnouncementText->SetColorAndOpacity(FLinearColor(1.0f, 0.9f, 0.2f, 1.0f)); 
        AnnouncementText->Font.Size = 96;
    }
    else
    {
        
        AnnouncementText->SetColorAndOpacity(FLinearColor::White);
        AnnouncementText->Font.Size = 64;
    }

    AnnouncementText->SetVisibility(ESlateVisibility::Visible);

    
    FTimerHandle HideTimer;
    GetWorld()->GetTimerManager().SetTimer(HideTimer, [this]()
        {
            if (AnnouncementText)
            {
                AnnouncementText->SetVisibility(ESlateVisibility::Hidden);
            }
        }, Duration, false);
}



void UGameHUD::ShowCountdown(int32 Seconds)
{
    if (!AnnouncementText || Seconds <= 0)
    {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT(">>> COUNTDOWN: %d <<<"), Seconds);

    
    FString CountText = FString::Printf(TEXT("%d"), Seconds);
    AnnouncementText->SetText(FText::FromString(CountText));
    AnnouncementText->SetColorAndOpacity(FLinearColor(1.0f, 0.2f, 0.2f, 1.0f)); 
    AnnouncementText->Font.Size = 120; 
    AnnouncementText->SetVisibility(ESlateVisibility::Visible);

    
    FTimerHandle HideTimer;
    GetWorld()->GetTimerManager().SetTimer(HideTimer, [this]()
        {
            if (AnnouncementText)
            {
                AnnouncementText->SetVisibility(ESlateVisibility::Hidden);
            }
        }, 0.8f, false);

    
    if (Seconds > 1)
    {
        FTimerHandle NextCountTimer;
        GetWorld()->GetTimerManager().SetTimer(NextCountTimer, [this, Seconds]()
            {
                ShowCountdown(Seconds - 1);
            }, 1.0f, false);
    }
}

void UGameHUD::UpdateEnemiesRemaining(int32 Killed, int32 Total)
{
    if (EnemiesRemainingText)
    {
        FString Text = FString::Printf(TEXT("Killed: %d / %d"), Killed, Total);
        EnemiesRemainingText->SetText(FText::FromString(Text));

       
        float Percent = (float)Killed / (float)Total;
        if (Percent >= 1.0f)
        {
            EnemiesRemainingText->SetColorAndOpacity(FLinearColor::Green);
        }
        else if (Percent >= 0.75f)
        {
            EnemiesRemainingText->SetColorAndOpacity(FLinearColor::Yellow);
        }
        else
        {
            EnemiesRemainingText->SetColorAndOpacity(FLinearColor::White);
        }
    }
}

void UGameHUD::UpdateWaveProgress(int32 Killed, int32 Remaining, int32 Total)
{
    if (EnemiesRemainingText)
    {
        FString Text = FString::Printf(TEXT("Killed: %d | Left: %d"), Killed, Remaining);
        EnemiesRemainingText->SetText(FText::FromString(Text));

        if (Remaining == 0)
        {
            EnemiesRemainingText->SetColorAndOpacity(FLinearColor::Green);
        }
        else if (Remaining <= 3)
        {
            EnemiesRemainingText->SetColorAndOpacity(FLinearColor::Yellow);
        }
        else
        {
            EnemiesRemainingText->SetColorAndOpacity(FLinearColor::White);
        }
    }
}

void UGameHUD::ShowBossHealthBar(const FString& BossName)
{
    UE_LOG(LogTemp, Warning, TEXT(">>> SHOWING BOSS HEALTH BAR <<<"));

    if (BossHealthBar)
    {
        BossHealthBar->SetVisibility(ESlateVisibility::Visible);
        BossHealthBar->SetPercent(1.0f);
        BossHealthBar->SetFillColorAndOpacity(FLinearColor(0.8f, 0.0f, 0.0f)); 
    }

    if (BossNameText)
    {
        BossNameText->SetText(FText::FromString(BossName));
        BossNameText->SetVisibility(ESlateVisibility::Visible);
        BossNameText->SetColorAndOpacity(FLinearColor(1.0f, 0.2f, 0.2f)); 
    }

    if (BossHealthText)
    {
        BossHealthText->SetVisibility(ESlateVisibility::Visible);
    }

    CurrentBossHealthPercent = 1.0f;
    TargetBossHealthPercent = 1.0f;
}

void UGameHUD::UpdateBossHealth(float CurrentHealth, float MaxHealth)
{
    if (!BossHealthBar || !BossHealthText)
    {
        return;
    }

    float HealthPercent = FMath::Clamp(CurrentHealth / MaxHealth, 0.0f, 1.0f);
    TargetBossHealthPercent = HealthPercent;

   
    BossHealthBar->SetPercent(HealthPercent);

    
    FString HealthString = FString::Printf(TEXT("%.0f / %.0f"), CurrentHealth, MaxHealth);
    BossHealthText->SetText(FText::FromString(HealthString));

    
    FLinearColor BarColor;
    if (HealthPercent > 0.6f)
    {
       
        BarColor = FLinearColor(0.8f, 0.0f, 0.0f);
    }
    else if (HealthPercent > 0.3f)
    {
       
        BarColor = FLinearColor(1.0f, 0.4f, 0.0f);
    }
    else
    {
       
        BarColor = FLinearColor(1.0f, 0.0f, 0.0f);
    }

    BossHealthBar->SetFillColorAndOpacity(BarColor);

    UE_LOG(LogTemp, Log, TEXT("Boss Health: %.0f/%.0f (%.1f%%)"),
        CurrentHealth, MaxHealth, HealthPercent * 100.0f);
}

void UGameHUD::HideBossHealthBar()
{
    if (BossHealthBar)
    {
        BossHealthBar->SetVisibility(ESlateVisibility::Hidden);
    }

    if (BossNameText)
    {
        BossNameText->SetVisibility(ESlateVisibility::Hidden);
    }

    if (BossHealthText)
    {
        BossHealthText->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UGameHUD::NativeConstruct()
{
    Super::NativeConstruct();

    UE_LOG(LogTemp, Warning, TEXT("=== GameHUD NativeConstruct ==="));

    if (BossHealthBar)
    {
        BossHealthBar->SetVisibility(ESlateVisibility::Hidden);
    }
    if (BossNameText)
    {
        BossNameText->SetVisibility(ESlateVisibility::Hidden);
    }
    if (BossHealthText)
    {
        BossHealthText->SetVisibility(ESlateVisibility::Hidden);
    }


    
    if (HealthBar)
    {
        UE_LOG(LogTemp, Log, TEXT("? HealthBar bound"));
    }
    else 
    {
        UE_LOG(LogTemp, Error, TEXT("? HealthBar NULL"));
    }

    if (HealthText) 
    {
        UE_LOG(LogTemp, Log, TEXT("? HealthText bound"));
    }
    else 
    {
        UE_LOG(LogTemp, Error, TEXT("? HealthText NULL"));
    }

    if (AmmoText) 
    {
        UE_LOG(LogTemp, Log, TEXT("? AmmoText bound"));
    }
    else 
    {
        UE_LOG(LogTemp, Error, TEXT("? AmmoText NULL"));
    }

    if (ScoreText) 
    {
        UE_LOG(LogTemp, Log, TEXT("? ScoreText bound"));
    }
    else 
    {
        UE_LOG(LogTemp, Error, TEXT("? ScoreText NULL"));
    }

    if (WeaponNameText) 
    {
        UE_LOG(LogTemp, Log, TEXT("? WeaponNameText bound"));
    }
    else 
    {
        UE_LOG(LogTemp, Error, TEXT("? WeaponNameText NULL"));
    }

   

  
    UpdateHealth(100, 100);
    UpdateAmmo("Pistol", 30, 30);
    UpdateScore(0);
    UpdateWeaponName("Pistol");

    UE_LOG(LogTemp, Warning, TEXT("==============================="));
}

void UGameHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (BossHealthBar && BossHealthBar->GetVisibility() == ESlateVisibility::Visible)
    {
        CurrentBossHealthPercent = FMath::FInterpTo(
            CurrentBossHealthPercent,
            TargetBossHealthPercent,
            InDeltaTime,
            5.0f  
        );

        BossHealthBar->SetPercent(CurrentBossHealthPercent);
    }
}
