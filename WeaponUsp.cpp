
#include "WeaponUsp.h"
#include "UObject/ConstructorHelpers.h"

AWeaponUsp::AWeaponUsp()
{
    WeaponName = "USP";
    FireRate = 0.3f;        
    Damage = 25.0f;
    ProjectilesPerShot = 1;
    SpreadAngle = 2.0f;
    MaxAmmo = 100;

    
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder"));
    if (CylinderMesh.Succeeded())
    {
        WeaponMesh->SetStaticMesh(CylinderMesh.Object);
        WeaponMesh->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.5f));
        WeaponMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));
    }
}
