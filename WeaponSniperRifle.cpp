
#include "WeaponSniperRifle.h"
#include "UObject/ConstructorHelpers.h"

AWeaponSniperRifle::AWeaponSniperRifle()
{
    WeaponName = "AWP";
    FireRate = 1.5f;        
    Damage = 50.0f;          
    ProjectilesPerShot = 1;  
    SpreadAngle = 0.0f;     
    MaxAmmo = 40;

    // Load a cube as shotgun mesh
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube"));
    if (CubeMesh.Succeeded())
    {
        WeaponMesh->SetStaticMesh(CubeMesh.Object);
        WeaponMesh->SetRelativeScale3D(FVector(1.0f, 0.2f, 0.2f));
    }
}
