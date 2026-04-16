
#include "MyArenaCharacter.h"


#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Camera/CameraShakeBase.h"


#include "BaseWeapon.h"
#include "GameHUD.h"
#include "GameOverWidget.h"
#include "MyGameMode.h"
#include "EnemySpawner.h" 
#include "WeaponType.h"



AMyArenaCharacter::AMyArenaCharacter()
{
	PrimaryActorTick.bCanEverTick = true;


	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;


	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 800.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 460.f;



	CurrentZoomDistance = 900.0f;
	TargetZoomDistance = 900.0f;


	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = CurrentZoomDistance;
	CameraBoom->SetRelativeRotation(FRotator(-70.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;


	TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCamera->bUsePawnControlRotation = false;

	/*
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube"));
	if (CubeMesh.Succeeded())
	{
		BodyMesh->SetStaticMesh(CubeMesh.Object);
		BodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		BodyMesh->SetRelativeScale3D(FVector(0.4f, 0.4f, 2.0f));
		BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}


	DirectionIndicator = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DirectionIndicator"));
	DirectionIndicator->SetupAttachment(BodyMesh);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMesh(TEXT("/Engine/BasicShapes/Cone"));
	if (ConeMesh.Succeeded())
	{
		DirectionIndicator->SetStaticMesh(ConeMesh.Object);
		DirectionIndicator->SetRelativeLocation(FVector(150.0f, 0.0f, 49.0f));
		DirectionIndicator->SetRelativeRotation(FRotator(0.0f, 90.0f, -90.0f));
		DirectionIndicator->SetRelativeScale3D(FVector(0.2f, 0.1f, 0.4f));
		DirectionIndicator->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	*/
	WeaponSocket = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponSocket"));

	
	WeaponSocket->SetupAttachment(GetMesh(), FName("hand_r"));
	WeaponSocket->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	WeaponSocket->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));


	
	static ConstructorHelpers::FObjectFinder<UInputAction> Fire(TEXT("/Game/Input/IA_Fire"));
	if (Fire.Succeeded())
	{
		FireAction = Fire.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> SwitchWeap(TEXT("/Game/Input/IA_SwitchWeapon"));
	if (SwitchWeap.Succeeded())
	{
		SwitchWeaponAction = SwitchWeap.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> Dash(TEXT("/Game/Input/IA_Dash"));
	if (Dash.Succeeded())
	{
		DashAction = Dash.Object;
	}

}

void AMyArenaCharacter::TakeDamage(float DamageAmount)
{
	if (bIsDead)
	{
		return;
	}

	if (bIsInvincible)
	{
		UE_LOG(LogTemp, Log, TEXT("Damage blocked by dash invincibility!"));
		return;
	}


	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);

	UE_LOG(LogTemp, Warning, TEXT("Player took %.0f damage! Health: %.0f/%.0f"),
		DamageAmount, CurrentHealth, MaxHealth);

	
	if (GameHUDWidget)
	{
		GameHUDWidget->UpdateHealth(CurrentHealth, MaxHealth);
	}

	
	FlashDamageEffect();

	if (DamageCameraShake)
	{
		APlayerController* PC = Cast<APlayerController>(Controller);
		if (PC && PC->PlayerCameraManager)
		{
			PC->PlayerCameraManager->StartCameraShake(DamageCameraShake, 1.0f);
		}
	}

	
	if (CurrentHealth <= 0.0f)
	{
		UE_LOG(LogTemp, Error, TEXT("PLAYER DIED!"));
		OnPlayerDeath();
	}
}

void AMyArenaCharacter::Heal(float HealAmount)
{
	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, MaxHealth);

	UE_LOG(LogTemp, Log, TEXT("Player healed %.0f! Health: %.0f/%.0f"),
		HealAmount, CurrentHealth, MaxHealth);

	
	if (GameHUDWidget)
	{
		GameHUDWidget->UpdateHealth(CurrentHealth, MaxHealth);
	}
}

void AMyArenaCharacter::AddScore(int32 Points)
{
	Score += Points;
	EnemiesKilled++;

	UE_LOG(LogTemp, Log, TEXT("Score +%d! Total: %d"), Points, Score);

	
	if (GameHUDWidget)
	{
		GameHUDWidget->UpdateScore(Score);
	}
}

void AMyArenaCharacter::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("========================================"));
	UE_LOG(LogTemp, Warning, TEXT("   CHARACTER BEGIN PLAY"));
	UE_LOG(LogTemp, Warning, TEXT("========================================"));


	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	
	if (BodyMesh)
	{
		ColorMaterial = BodyMesh->CreateDynamicMaterialInstance(0);
		if (ColorMaterial)
		{
			ColorMaterial->SetVectorParameterValue(FName("Color"), FLinearColor::Blue);
		}
	}

	
	for (TSubclassOf<ABaseWeapon> WeaponClass : WeaponClasses)
	{
		if (WeaponClass)
		{
			FVector SpawnLocation = WeaponSocket->GetComponentLocation();
			FRotator SpawnRotation = WeaponSocket->GetComponentRotation();

			ABaseWeapon* Weapon = GetWorld()->SpawnActor<ABaseWeapon>(WeaponClass, SpawnLocation, SpawnRotation);

			if (Weapon)
			{
				Weapon->AttachToComponent(WeaponSocket, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				Weapon->SetOwner(this);
				Weapon->SetActorHiddenInGame(true);
				WeaponInventory.Add(Weapon);

				UE_LOG(LogTemp, Log, TEXT("Weapon spawned: %s (Ammo: %d/%d)"),
					*Weapon->GetWeaponName(),
					Weapon->GetCurrentAmmo(),
					Weapon->GetMaxAmmo());
			}
		}
	}

	
	if (WeaponInventory.Num() > 0)
	{
		CurrentWeapon = WeaponInventory[0];
		CurrentWeapon->SetActorHiddenInGame(false);
		CurrentWeaponIndex = 0;

		UE_LOG(LogTemp, Warning, TEXT("Starting weapon: %s"), *CurrentWeapon->GetWeaponName());

		CurrentWeaponAnimType = CurrentWeapon->GetWeaponType();

		UE_LOG(LogTemp, Warning, TEXT("Starting weapon: %s | Anim Type: %d"),
			*CurrentWeapon->GetWeaponName(),
			(int32)CurrentWeaponAnimType);
	}

	
	FTimerHandle InitHUDTimer;
	GetWorld()->GetTimerManager().SetTimer(InitHUDTimer, [this]()
		{
			if (GameHUDWidget && CurrentWeapon)
			{
				UE_LOG(LogTemp, Warning, TEXT("Initializing HUD with weapon info"));

				GameHUDWidget->UpdateAmmo(
					CurrentWeapon->GetWeaponName(),
					CurrentWeapon->GetCurrentAmmo(),
					CurrentWeapon->GetMaxAmmo()
				);

				GameHUDWidget->UpdateHealth(CurrentHealth, MaxHealth);
				GameHUDWidget->UpdateScore(Score);
			}
		}, 0.5f, false);



}

void AMyArenaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateTowardsMouse();
	UpdateCameraZoom(DeltaTime);


	if (bIsDead)
	{
		return;
	}


	SurvivalTime += DeltaTime;


	APlayerController* PC = Cast<APlayerController>(Controller);
	if (PC)
	{
		if (PC->WasInputKeyJustPressed(EKeys::H))
		{
			TakeDamage(10);
		}
		if (PC->WasInputKeyJustPressed(EKeys::J))
		{
			Heal(20);
		}
		if (PC->WasInputKeyJustPressed(EKeys::K))
		{
			AddScore(100);
		}
	}
}

void AMyArenaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyArenaCharacter::Move);
			UE_LOG(LogTemp, Warning, TEXT("Move Action Bound"));
		}

		if (ShootAction)
		{
			EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &AMyArenaCharacter::Shoot);
			UE_LOG(LogTemp, Warning, TEXT("Shoot Action Bound"));
		}

		
		if (ZoomAction)
		{
			EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &AMyArenaCharacter::Zoom);
			UE_LOG(LogTemp, Warning, TEXT("Zoom Action Bound"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ZoomAction is NULL!"));
		}

		
		if (FireAction)
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &AMyArenaCharacter::FireWeapon);
			UE_LOG(LogTemp, Log, TEXT("✓ Fire Action Bound"));
		}

		
		if (SwitchWeaponAction)
		{
			EnhancedInputComponent->BindAction(SwitchWeaponAction, ETriggerEvent::Started, this, &AMyArenaCharacter::SwitchWeapon);
			UE_LOG(LogTemp, Log, TEXT("✓ Switch Weapon Action Bound"));
		}

		if (DashAction)
		{
			EnhancedInputComponent->BindAction(
				DashAction, ETriggerEvent::Started, this, &AMyArenaCharacter::Dash);
			UE_LOG(LogTemp, Log, TEXT("✓ Dash Action Bound"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("✗ DashAction is NULL!"));
		}
	}
}

void AMyArenaCharacter::FlashDamageEffect()
{
	
	if (ColorMaterial)
	{
		ColorMaterial->SetVectorParameterValue(FName("Color"), FLinearColor::Red);

		
		FTimerHandle ResetColorTimer;
		GetWorld()->GetTimerManager().SetTimer(ResetColorTimer, [this]()
			{
				if (ColorMaterial)
				{
					ColorMaterial->SetVectorParameterValue(FName("Color"), FLinearColor::Blue);
				}
			}, 0.2f, false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ColorMaterial is NULL, can't flash damage"));
	}
}

void AMyArenaCharacter::OnPlayerDeath()
{
	if (bIsDead == false)
	{
		bIsDead = true;
	}


	UE_LOG(LogTemp, Error, TEXT("=== PLAYER DIED ==="));
	

	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();

	
	APlayerController* PC = Cast<APlayerController>(Controller);
	if (PC)
	{
		DisableInput(PC);
	}


	if (CurrentWeapon)
	{
		
		CurrentWeapon->SetActorHiddenInGame(true);
	}

	
	AMyGameMode* GameMode = Cast<AMyGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode && GameMode->GetWaveManager())
	{
		
		GetWorld()->GetTimerManager().ClearAllTimersForObject(GameMode->GetWaveManager());
		UE_LOG(LogTemp, Log, TEXT("✓ Wave system stopped"));
	}

	
	FTimerHandle GameOverTimer;
	GetWorld()->GetTimerManager().SetTimer(GameOverTimer, [this]()
		{
			if (GameHUDWidget)
			{
				GameHUDWidget->RemoveFromParent();
				UE_LOG(LogTemp, Log, TEXT("✓ Game HUD hidden"));
			}


			ShowGameOverScreen();
		}, .8f, false); 
}

void AMyArenaCharacter::ShowGameOverScreen()
{
	APlayerController* PC = Cast<APlayerController>(Controller);
	if (!PC)
	{
		return;
	}

	
	TSubclassOf<UGameOverWidget> GameOverClass = LoadClass<UGameOverWidget>(nullptr,
		TEXT("/Game/UI/WBP_GameOver.WBP_GameOver_C"));

	if (GameOverClass)
	{
		UGameOverWidget* GameOverWidget = CreateWidget<UGameOverWidget>(PC, GameOverClass);
		if (GameOverWidget)
		{
			
			GameOverWidget->ShowGameOver(Score, EnemiesKilled, SurvivalTime);
			GameOverWidget->AddToViewport(100); 

			
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(GameOverWidget->TakeWidget());
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = true;

			UE_LOG(LogTemp, Warning, TEXT("✓ Game Over screen displayed"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("✗ Failed to create Game Over widget"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("✗ Failed to load WBP_GameOver"));
	}
}

void AMyArenaCharacter::Move(const FInputActionValue & Value)
{
	if (bIsDead) return;


	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
			
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

			
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

			
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMyArenaCharacter::Shoot(const FInputActionValue& Value)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Shoot!"));
	}
}

void AMyArenaCharacter::Zoom(const FInputActionValue& Value)
{
	if (bIsDead) return;


	float ScrollValue = Value.Get<float>();

	
	TargetZoomDistance -= ScrollValue * ZoomSpeed;

	
	TargetZoomDistance = FMath::Clamp(TargetZoomDistance, MinZoomDistance, MaxZoomDistance);

	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 0.1f, FColor::Cyan,
			FString::Printf(TEXT("Zoom: %.0f"), TargetZoomDistance));
	}
}

void AMyArenaCharacter::UpdateCameraZoom(float DeltaTime)
{
	if (bIsDead) return;


	if (!CameraBoom) return;

	
	CurrentZoomDistance = FMath::FInterpTo(CurrentZoomDistance,TargetZoomDistance,DeltaTime,SmothZoomSpeed);

	
	CameraBoom->TargetArmLength = CurrentZoomDistance;

	
	float ZoomAlpha = (CurrentZoomDistance - MinZoomDistance) / (MaxZoomDistance - MinZoomDistance);
	float TargetAngle = FMath::Lerp(MaxZoomAngle, MinZoomAngle, ZoomAlpha);

	
	CameraBoom->SetRelativeRotation(FRotator(TargetAngle, 0.f, 0.0f));
}

void AMyArenaCharacter::RotateTowardsMouse()
{
	if (bIsDead) return;


	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		FHitResult HitResult;
		PC->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

		if (HitResult.bBlockingHit)
		{
			
			FVector Direction = HitResult.Location - GetActorLocation();
			Direction.Z = 0.0f;

			if (!Direction.IsNearlyZero())
			{
				FRotator NewRotation = Direction.Rotation();
				SetActorRotation(NewRotation);
			}
		}
	}
}

void AMyArenaCharacter::FireWeapon(const FInputActionValue& Value)
{
	if (!CurrentWeapon)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot fire - no current weapon!"));
		return;
	}

	if (bIsDead)
	{
		return;
	}

	bool bIsPressed = Value.Get<bool>();

	if (bIsPressed)
	{
		
			CurrentWeapon->GetWeaponName(),
			CurrentWeapon->GetCurrentAmmo(),
			CurrentWeapon->GetMaxAmmo();

		CurrentWeapon->Fire();

		
		
			CurrentWeapon->GetWeaponName(),
			CurrentWeapon->GetCurrentAmmo(),
			CurrentWeapon->GetMaxAmmo();
	}
}

void AMyArenaCharacter::SwitchWeapon(const FInputActionValue& Value)
{
	if (WeaponInventory.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No weapons in inventory!"));
		return;
	}

	float AxisValue = Value.Get<float>();

	
	if (FMath::Abs(AxisValue) < 0.1f)
	{
		return;
	}

	int32 OldIndex = CurrentWeaponIndex;

	if (AxisValue > 0.1f) 
	{
		CurrentWeaponIndex++;
		if (CurrentWeaponIndex >= WeaponInventory.Num())
		{
			CurrentWeaponIndex = 0;
		}
	}
	else if (AxisValue < -0.1f) 
	{
		CurrentWeaponIndex--;
		if (CurrentWeaponIndex < 0)
		{
			CurrentWeaponIndex = WeaponInventory.Num() - 1;
		}
	}

	
	if (OldIndex == CurrentWeaponIndex)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("========================================"));
	UE_LOG(LogTemp, Warning, TEXT("   SWITCHING WEAPON: %d -> %d"), OldIndex, CurrentWeaponIndex);
	UE_LOG(LogTemp, Warning, TEXT("========================================"));

	
	if (CurrentWeapon)
	{
		CurrentWeapon->SetActorHiddenInGame(true);
		UE_LOG(LogTemp, Log, TEXT("Hidden: %s"), *CurrentWeapon->GetWeaponName());
	}

	
	CurrentWeapon = WeaponInventory[CurrentWeaponIndex];

	if (CurrentWeapon)
	{
		CurrentWeapon->SetActorHiddenInGame(false);
		CurrentWeaponAnimType = CurrentWeapon->GetWeaponType();

		UE_LOG(LogTemp, Warning, TEXT("✓ Equipped: %s"), *CurrentWeapon->GetWeaponName());
		UE_LOG(LogTemp, Warning, TEXT("   Ammo: %d/%d"),
			CurrentWeapon->GetCurrentAmmo(),
			CurrentWeapon->GetMaxAmmo());

		
		if (GameHUDWidget)
		{
			GameHUDWidget->UpdateAmmo(
				CurrentWeapon->GetWeaponName(),
				CurrentWeapon->GetCurrentAmmo(),
				CurrentWeapon->GetMaxAmmo()
			);

			UE_LOG(LogTemp, Log, TEXT("✓ HUD updated with weapon info"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("✗ GameHUDWidget is NULL!"));
		}
	}
}

void AMyArenaCharacter::EquipWeapon(int32 WeaponIndex)
{
	if (!WeaponInventory.IsValidIndex(WeaponIndex))
	{
		return;
	}

	
	if (CurrentWeapon)
	{
		CurrentWeapon->SetActorHiddenInGame(true);
	}

	
	CurrentWeapon = WeaponInventory[WeaponIndex];
	CurrentWeapon->SetActorHiddenInGame(false);
	CurrentWeaponIndex = WeaponIndex;

	UE_LOG(LogTemp, Warning, TEXT("Equipped: %s"), *CurrentWeapon->WeaponName);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(2, 2.0f, FColor::Yellow,
			FString::Printf(TEXT("Equipped: %s"), *CurrentWeapon->WeaponName));
	}
}

void AMyArenaCharacter::SpawnWeapons()
{
	if (bIsDead) return;


	if (WeaponClasses.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No weapon classes assigned to character!"));
		return;
	}

	UWorld* World = GetWorld();
	if (!World) return;

	
	for (TSubclassOf<ABaseWeapon> WeaponClass : WeaponClasses)
	{
		if (!WeaponClass) continue;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;

		ABaseWeapon* Weapon = World->SpawnActor<ABaseWeapon>(WeaponClass, SpawnParams);
		if (Weapon)
		{
			Weapon->AttachToComponent(WeaponSocket, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			Weapon->SetActorHiddenInGame(true); 
			WeaponInventory.Add(Weapon);

			UE_LOG(LogTemp, Log, TEXT("✓ Spawned weapon: %s"), *Weapon->WeaponName);
		}
	}

	
	if (WeaponInventory.Num() > 0)
	{
		EquipWeapon(0);
	}
}

void AMyArenaCharacter::Dash()
{
	if (bIsDead || !bCanDash || bIsDashing) return;

	
	FVector DashDirection = GetLastMovementInputVector();

	if (DashDirection.IsNearlyZero())
	{
		
		DashDirection = GetActorForwardVector();
	}

	
	DashDirection.Z = 0.0f;
	DashDirection = DashDirection.GetSafeNormal();

	UE_LOG(LogTemp, Log, TEXT("Dash! Direction: %s | Force: %.0f"),
		*DashDirection.ToString(), DashForce);


	bIsDashing = true;
	bCanDash = false;
	bIsInvincible = true;

	
	LaunchCharacter(DashDirection * DashForce, true, false);

	
	if (ColorMaterial)
	{
		ColorMaterial->SetVectorParameterValue(
			FName("Color"), FLinearColor(0.0f, 1.0f, 1.0f, 1.0f));
	}

	
	GetWorld()->GetTimerManager().SetTimer(DashDurationTimer, [this]()
		{
			bIsDashing = false;
			UE_LOG(LogTemp, Log, TEXT("Dash movement complete"));
		}, DashDuration, false);


	GetWorld()->GetTimerManager().SetTimer(DashInvincibilityTimer, [this]()
		{
			bIsInvincible = false;

			
			if (ColorMaterial)
			{
				ColorMaterial->SetVectorParameterValue(
					FName("Color"), FLinearColor::Blue);
			}

			UE_LOG(LogTemp, Log, TEXT("Dash invincibility ended"));
		}, DashInvincibilityDuration, false);

	
	GetWorld()->GetTimerManager().SetTimer(DashCooldownTimer, [this]()
		{
			bCanDash = true;
			UE_LOG(LogTemp, Log, TEXT("Dash ready"));
		}, DashCooldown, false);
}
