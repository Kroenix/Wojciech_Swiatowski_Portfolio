
#include "HealthPickup.h"


#include "MyArenaCharacter.h"

AHealthPickup::AHealthPickup()
{
 
    PickupColor = FLinearColor(0.0f, 1.0f, 0.0f, 1.0f);

    HealAmount = 25.0f;
}

void AHealthPickup::OnPickedUp(AMyArenaCharacter* Character)
{
    if (Character)
    {
        Character->Heal(HealAmount);

        UE_LOG(LogTemp, Warning, TEXT(">>> Health Pickup! +%.0f HP <<<"), HealAmount);
    }
}

