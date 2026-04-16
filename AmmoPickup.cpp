
#include "AmmoPickup.h"


#include "MyArenaCharacter.h"
#include "BaseWeapon.h"

AAmmoPickup::AAmmoPickup()
{
    
    PickupColor = FLinearColor(1.0f, 0.8f, 0.0f, 1.0f);

    AmmoAmount = 30;
}

void AAmmoPickup::OnPickedUp(AMyArenaCharacter* Character)
{
    if (Character)
    {
        
        ABaseWeapon* CurrentWeapon = Character->GetCurrentWeapon();
        if (CurrentWeapon)
        {
            CurrentWeapon->AddAmmo(AmmoAmount);

            UE_LOG(LogTemp, Warning, TEXT(">>> Ammo Pickup! +%d ammo for %s <<<"),
                AmmoAmount, *CurrentWeapon->GetWeaponName());
        }
    }
}
