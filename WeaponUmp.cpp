
#include "WeaponUmp.h"
#include "UObject/ConstructorHelpers.h"

AWeaponUmp::AWeaponUmp()
{
    WeaponName = "UMP";
    FireRate = 0.2f;        
    Damage = 12.0f;         
    ProjectilesPerShot = 1;
    SpreadAngle = 3.0f;     
    MaxAmmo = 120;

    
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube"));
    if (CubeMesh.Succeeded())
    {
        WeaponMesh->SetStaticMesh(CubeMesh.Object);
        WeaponMesh->SetRelativeScale3D(FVector(0.8f, 0.15f, 0.15f));
    }
}
