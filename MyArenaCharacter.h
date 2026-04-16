
#pragma once

#include "CoreMinimal.h"
#include "WeaponType.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "BaseWeapon.h"
#include "MyArenaCharacter.generated.h"


UCLASS()
class ARENA_API AMyArenaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	
	AMyArenaCharacter();


	virtual void Tick(float DeltaTime) override;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* MoveAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* ShootAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* ZoomAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* FireAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* SwitchWeaponAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputAction* DashAction;



	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TArray<TSubclassOf<class ABaseWeapon>> WeaponClasses;
	UPROPERTY()
	TArray<class ABaseWeapon*> WeaponInventory;


	
	UFUNCTION(BlueprintCallable, Category = "Health")
	void TakeDamage(float DamageAmount);
	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal(float HealAmount);
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercent() const { return CurrentHealth / MaxHealth; }
	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const { return bIsDead; }


	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Points);

	// Get HUD
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetGameHUD(UGameHUD* HUD) { GameHUDWidget = HUD; }


	UFUNCTION(BlueprintPure, Category = "Dash")
	bool IsDashing() const { return bIsDashing; }

	UFUNCTION(BlueprintPure, Category = "Dash")
	bool CanDash() const { return bCanDash; }
	


	UFUNCTION(BlueprintPure, Category = "Stats")
	int32 GetEnemiesKilled() const { return EnemiesKilled; }
	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetSurvivalTime() const { return SurvivalTime; }
	UFUNCTION(BlueprintPure, Category = "Stats")
	int32 GetScore() const { return Score; }
	UFUNCTION(BlueprintPure, Category = "HUD")
	UGameHUD* GetGameHUD() const { return GameHUDWidget; }
	UFUNCTION(BlueprintPure, Category = "Weapon")
	ABaseWeapon* GetCurrentWeapon() const { return CurrentWeapon; }
	UFUNCTION(BlueprintPure, Category = "Weapon")
	int32 GetCurrentWeaponIndex() const { return CurrentWeaponIndex; }


	UFUNCTION(BlueprintPure, Category = "Animation")
	EWeaponType GetCurrentWeaponAnimationType() const { return CurrentWeaponAnimType; }


protected:

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)override;

	void FlashDamageEffect();
	void OnPlayerDeath();
	void ShowGameOverScreen();


	UPROPERTY(BlueprintReadOnly, Category = "Health")
	bool bIsDead = false;

	// Health system
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float MaxHealth = 100.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Health")
	float CurrentHealth = 100.0f;

	// Score
	UPROPERTY(BlueprintReadOnly, Category = "Score")
	int32 Score = 0;


	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	int32 EnemiesKilled = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float SurvivalTime = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	EWeaponType CurrentWeaponAnimType = EWeaponType::Pistol;

	UPROPERTY()
	class UGameHUD* GameHUDWidget;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCamera;


	UPROPERTY(VIsibleAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float MinZoomDistance = 300.f;
	UPROPERTY(VIsibleAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float MaxZoomDistance = 1200.f;
	UPROPERTY(VIsibleAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float MinZoomAngle = -70.f;
	UPROPERTY(VIsibleAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float MaxZoomAngle = -40.f;
	UPROPERTY(VIsibleAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float ZoomSpeed = 20.f;
	UPROPERTY(VIsibleAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float SmothZoomSpeed = 3.f;


	float CurrentZoomDistance = 900.f;
	float TargetZoomDistance = 900.f;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BodyMesh;  
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* DirectionIndicator;  


	UPROPERTY()
	class ABaseWeapon* CurrentWeapon;
	UPROPERTY()
	int32 CurrentWeaponIndex = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* WeaponSocket;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<class UCameraShakeBase> DamageCameraShake;
	


	void Move(const FInputActionValue& Value);
	void Zoom(const FInputActionValue& Value);
	void Shoot(const FInputActionValue& Value);


	void UpdateCameraZoom(float DeltaTime);


	void RotateTowardsMouse();




	void FireWeapon(const FInputActionValue& Value);
	void SwitchWeapon(const FInputActionValue& Value);
	void EquipWeapon(int32 WeaponIndex);
	void SpawnWeapons();


	UPROPERTY()
	class UMaterialInstanceDynamic* ColorMaterial;












	// Dash settings
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float DashForce = 2000.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float DashCooldown = 3.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float DashDuration = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float DashInvincibilityDuration = 0.5f;

	// Dash state
	bool bIsDashing = false;
	bool bCanDash = true;
	bool bIsInvincible = false;

	// Dash timers
	FTimerHandle DashDurationTimer;
	FTimerHandle DashInvincibilityTimer;
	FTimerHandle DashCooldownTimer;

	// Dash function
	void Dash();
};
