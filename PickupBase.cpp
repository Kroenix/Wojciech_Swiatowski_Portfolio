#include "PickupBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "MyArenaCharacter.h"
#include "Kismet/GameplayStatics.h"

APickupBase::APickupBase()
{
    PrimaryActorTick.bCanEverTick = true;

    
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    RootComponent = CollisionSphere;
    CollisionSphere->SetSphereRadius(50.0f);
    CollisionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

    
    PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
    PickupMesh->SetupAttachment(CollisionSphere);
    PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube"));
    if (CubeMesh.Succeeded())
    {
        PickupMesh->SetStaticMesh(CubeMesh.Object);
        PickupMesh->SetWorldScale3D(FVector(0.5f));
    }
}

void APickupBase::BeginPlay()
{
    Super::BeginPlay();

    StartLocation = GetActorLocation();

    
    CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &APickupBase::OnOverlapBegin);

    
    if (PickupMesh)
    {
        DynamicMaterial = PickupMesh->CreateDynamicMaterialInstance(0);
        if (DynamicMaterial)
        {
            DynamicMaterial->SetVectorParameterValue(FName("Color"), PickupColor);
        }
    }
}

void APickupBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    
    if (bRotate)
    {
        FRotator NewRotation = GetActorRotation();
        NewRotation.Yaw += RotationSpeed * DeltaTime;
        SetActorRotation(NewRotation);
    }

   
    if (bBobUpDown)
    {
        BobTime += DeltaTime * BobSpeed;
        float ZOffset = FMath::Sin(BobTime) * BobAmount;
        FVector NewLocation = StartLocation + FVector(0.0f, 0.0f, ZOffset);
        SetActorLocation(NewLocation);
    }
}

void APickupBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AMyArenaCharacter* Character = Cast<AMyArenaCharacter>(OtherActor);
    if (Character)
    {
        
        OnPickedUp(Character);

        
        if (PickupSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
        }

        
        Destroy();
    }
}

void APickupBase::OnPickedUp(AMyArenaCharacter* Character)
{
    
    UE_LOG(LogTemp, Log, TEXT("Pickup collected"));
}