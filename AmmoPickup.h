
#pragma once

#include "CoreMinimal.h"
#include "PickupBase.h"
#include "AmmoPickup.generated.h"


UCLASS()
class ARENA_API AAmmoPickup : public APickupBase
{
	GENERATED_BODY()
	

public:
    AAmmoPickup();

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Ammo")
    int32 AmmoAmount = 30;

    virtual void OnPickedUp(class AMyArenaCharacter* Character) override;

};
