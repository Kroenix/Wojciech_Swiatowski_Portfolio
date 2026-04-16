#include "MyGameMode.h"
#include "MyArenaCharacter.h"
#include "MyPlayerController.h"
#include "ArenaGameInstance.h"
#include "WaveManager.h"
#include "EnemyBase.h"
#include "BossEnemy.h"
#include "FadeWidget.h"
#include "Components/Image.h"
#include "GameHUD.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkyLight.h"


AMyGameMode::AMyGameMode()
{
    static ConstructorHelpers::FClassFinder<APawn> CharacterBP(
        TEXT("/Game/BP_ArenaCharacter.BP_ArenaCharacter_C"));
    if (CharacterBP.Class)
    {
        DefaultPawnClass = CharacterBP.Class;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("MyGameMode Initialized C++"));
        DefaultPawnClass = AMyArenaCharacter::StaticClass();
    }

    PlayerControllerClass = AMyPlayerController::StaticClass();

    UE_LOG(LogTemp, Warning, TEXT("MyGameMode Initialized"));
}

void AMyGameMode::BeginPlay()
{
    Super::BeginPlay();


    UE_LOG(LogTemp, Warning, TEXT("========================================"));
    UE_LOG(LogTemp, Warning, TEXT("   GAME LEVEL - MyGameMode BeginPlay"));
    UE_LOG(LogTemp, Warning, TEXT("========================================"));


    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    if (PC && PC->PlayerCameraManager)
    {
        PC->PlayerCameraManager->SetManualCameraFade(1.0f, FLinearColor::Black, false);
        UE_LOG(LogTemp, Warning, TEXT("✓ Screen set to BLACK immediately"));
    }

    UGameHUD* HUDWidget = nullptr;


    if (PC)
    {

      
        FInputModeGameAndUI InputMode;
        InputMode.SetHideCursorDuringCapture(false);
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(InputMode);

        PC->bShowMouseCursor = true;
        PC->bEnableClickEvents = true;
        PC->bEnableMouseOverEvents = true;

        UE_LOG(LogTemp, Warning, TEXT("✓ Game input mode set - WASD should work"));

      
        TSubclassOf<UGameHUD> GameHUDClass = LoadClass<UGameHUD>(nullptr,
            TEXT("/Game/UI/WBP_GameHUD.WBP_GameHUD_C"));

        if (GameHUDClass)
        {
            HUDWidget = CreateWidget<UGameHUD>(PC, GameHUDClass);
            if (HUDWidget)
            {
                HUDWidget->AddToViewport(0);
               
                UE_LOG(LogTemp, Log, TEXT("✓ HUD created"));

                AMyArenaCharacter* Character = Cast<AMyArenaCharacter>(PC->GetPawn());
                if (Character)
                {
                    Character->SetGameHUD(HUDWidget);

                    
                    HUDWidget->UpdateHealth(Character->GetHealthPercent() * 100.0f, 100.0f);
                    HUDWidget->UpdateScore(0);
                 
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("✗ Failed to load WBP_GameHUD"));
        }
    }





    FVector SpawnerLocation = FVector(0, 0, 100);

  
    TSubclassOf<AWaveManager> WaveManagerClass = LoadClass<AWaveManager>(nullptr,
        TEXT("/Game/Blueprints/BP_WaveManager.BP_WaveManager_C"));

    if (!WaveManagerClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("BP_WaveManager not found, using C++ class"));
        WaveManagerClass = AWaveManager::StaticClass();
    }

    WaveManager = GetWorld()->SpawnActor<AWaveManager>(WaveManagerClass, SpawnerLocation, FRotator::ZeroRotator);

    if (WaveManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("✓ Wave Manager created"));

       
        if (!WaveManager->EnemyClass)
        {
            UE_LOG(LogTemp, Warning, TEXT("EnemyClass not set, loading BP_Enemy..."));

            TSubclassOf<AEnemyBase> EnemyBP = LoadClass<AEnemyBase>(nullptr,
                TEXT("/Game/Blueprints/BP_Enemy.BP_Enemy_C"));

            if (EnemyBP)
            {
                WaveManager->EnemyClass = EnemyBP;
                UE_LOG(LogTemp, Warning, TEXT("✓ Enemy Class set to BP_Enemy"));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("✗ Failed to load BP_Enemy"));
                WaveManager->EnemyClass = AEnemyBase::StaticClass();
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("✓ EnemyClass already set: %s"),
                *WaveManager->EnemyClass->GetName());
        }

        
        if (!WaveManager->BossEnemyClass)
        {
            UE_LOG(LogTemp, Warning, TEXT("BossEnemyClass not set, loading BP_BossEnemy..."));

           
            TSubclassOf<ABossEnemy> BossBP = LoadClass<ABossEnemy>(nullptr,
                TEXT("/Game/Blueprints/BP_BossEnemy.BP_BossEnemy_C"));

            if (BossBP)
            {
                WaveManager->BossEnemyClass = BossBP;
                UE_LOG(LogTemp, Error, TEXT("✓✓✓ Boss Class set to BP_BossEnemy ✓✓✓"));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("✗ Failed to load BP_BossEnemy, using C++ class"));
                WaveManager->BossEnemyClass = ABossEnemy::StaticClass();
                UE_LOG(LogTemp, Warning, TEXT("✓ Boss Class set to C++ ABossEnemy"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("✓ BossEnemyClass already set: %s"),
                *WaveManager->BossEnemyClass->GetName());
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("✗ Failed to create Wave Manager"));
    }


    UArenaGameInstance* GameInstance = Cast<UArenaGameInstance>(GetGameInstance());
    if (GameInstance)
    {
       
        FTimerHandle InitialDelayTimer;
        GetWorld()->GetTimerManager().SetTimer(InitialDelayTimer, [GameInstance, PC]()
            {
                UE_LOG(LogTemp, Warning, TEXT("Creating fade widget for level start..."));

                TSubclassOf<UFadeWidget> FadeClass = LoadClass<UFadeWidget>(nullptr,
                    TEXT("/Game/UI/WBP_Fade.WBP_Fade_C"));

                if (FadeClass)
                {
                    UFadeWidget* TempFadeWidget = CreateWidget<UFadeWidget>(PC, FadeClass);
                    if (TempFadeWidget)
                    {
                        TempFadeWidget->AddToViewport(10000);

                        if (TempFadeWidget->GetFadeImage())
                        {
                           
                            TempFadeWidget->GetFadeImage()->SetRenderOpacity(1.0f);
                            TempFadeWidget->GetFadeImage()->SetColorAndOpacity(FLinearColor::Black);
                            UE_LOG(LogTemp, Warning, TEXT("✓ Black screen created"));

                            
                            TempFadeWidget->FadeFromBlack(1.0f);

                         
                            FTimerHandle RemoveTimer;
                            GameInstance->GetWorld()->GetTimerManager().SetTimer(RemoveTimer, [TempFadeWidget]()
                                {
                                    if (TempFadeWidget)
                                    {
                                        TempFadeWidget->RemoveFromParent();
                                    }
                                    UE_LOG(LogTemp, Warning, TEXT("✓ Fade complete!"));
                                }, 1.5f, false);
                        }
                    }
                }

            }, 0.1f, false);
    }


   
    if (PC && PC->PlayerCameraManager)
    {
        FTimerHandle FadeTimer;
        GetWorld()->GetTimerManager().SetTimer(FadeTimer, [PC]()
            {
                PC->PlayerCameraManager->StartCameraFade(
                    1.0f, 
                    0.0f, 
                    1.5f, 
                    FLinearColor::Black,
                    false,
                    false
                );
                UE_LOG(LogTemp, Warning, TEXT("✓ Fading in (1.5 sec)"));
            }, 0.3f, false); 
    }


}

