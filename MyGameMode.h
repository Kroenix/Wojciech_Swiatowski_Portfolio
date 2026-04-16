
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MyGameMode.generated.h"

UCLASS()
class ARENA_API AMyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyGameMode();


	UFUNCTION(BlueprintPure, Category = "Waves")
	AWaveManager* GetWaveManager() const { return WaveManager; }


protected:
	virtual void BeginPlay() override;


	UPROPERTY(EditDefaultsOnly, Category = "Enemies")
		TSubclassOf<class AEnemySpawner> EnemySpawnerClass;


	UPROPERTY()
	class AWaveManager* WaveManager;



};