

#include "WeaponShotgun.h"
#include "UObject/ConstructorHelpers.h"

AWeaponShotgun::AWeaponShotgun()
{
    WeaponName = "Shotgun";
    FireRate = 0.8f;         
    Damage = 12.0f;          
    ProjectilesPerShot = 8;  
    SpreadAngle = 15.0f;     
    MaxAmmo = 60;

    
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube"));
    if (CubeMesh.Succeeded())
    {
        WeaponMesh->SetStaticMesh(CubeMesh.Object);
        WeaponMesh->SetRelativeScale3D(FVector(1.0f, 0.2f, 0.2f));
    }
}
