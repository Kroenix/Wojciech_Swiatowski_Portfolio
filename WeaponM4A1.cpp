
#include "WeaponM4A1.h"
#include "UObject/ConstructorHelpers.h"

AWeaponM4A1::AWeaponM4A1()
{
    WeaponName = "M4A1";
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
