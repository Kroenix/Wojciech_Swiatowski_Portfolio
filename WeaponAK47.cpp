
#include "WeaponAK47.h"
#include "UObject/ConstructorHelpers.h"

AWeaponAK47::AWeaponAK47()
{
    WeaponName = "AK-47";
    FireRate = 0.1f;        
    Damage = 8.0f;        
    ProjectilesPerShot = 1;
    SpreadAngle = 1.0f;
    MaxAmmo = 80;

    
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube"));
    if (CubeMesh.Succeeded())
    {
        WeaponMesh->SetStaticMesh(CubeMesh.Object);
        WeaponMesh->SetRelativeScale3D(FVector(0.8f, 0.15f, 0.15f));
    }
}
