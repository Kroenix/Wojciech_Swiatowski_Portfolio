
#include "EnemyTank.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

AEnemyTank::AEnemyTank()
{
    
    MaxHealth = 300.0f;
    MoveSpeed = 150.0f;
    AttackDamage = 25.0f;
    AttackCooldown = 2.0f;
    ScoreValue = 300;

    
    EnemyColor = FLinearColor(0.35f, 0.0f, 0.0f, 1.0f);

    
    GetCapsuleComponent()->SetCapsuleHalfHeight(50.0f);
    GetCapsuleComponent()->SetCapsuleRadius(55.0f);
}
