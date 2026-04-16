
#pragma once

#include "CoreMinimal.h"
#include "PickupBase.h"
#include "HealthPickup.generated.h"


UCLASS()
class ARENA_API AHealthPickup : public APickupBase
{
	GENERATED_BODY()
	
public:
    AHealthPickup();

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Health")
    float HealAmount = 20.0f;

    virtual void OnPickedUp(class AMyArenaCharacter* Character) override;


};
