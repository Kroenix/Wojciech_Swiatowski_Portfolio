
#include "EnemyFast.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

AEnemyFast::AEnemyFast()
{
   
    MaxHealth = 50.0f;
    MoveSpeed = 600.0f;
    AttackDamage = 7.0f;
    AttackCooldown = 0.7f;
    ScoreValue = 75;

   
    EnemyColor = FLinearColor(0.0f, 0.4f, 1.0f, 1.0f);

    
    GetCapsuleComponent()->SetCapsuleHalfHeight(50.0f);
    GetCapsuleComponent()->SetCapsuleRadius(28.0f);
}
