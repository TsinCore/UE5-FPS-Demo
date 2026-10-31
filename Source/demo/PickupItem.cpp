#include "PickupItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

APickupItem::APickupItem()
{
    PrimaryActorTick.bCanEverTick = true;

    SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    SphereComponent->InitSphereRadius(80.f);
    SphereComponent->SetCollisionProfileName(TEXT("Trigger"));
    SphereComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    RootComponent = SphereComponent;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(RootComponent);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APickupItem::BeginPlay()
{
    Super::BeginPlay();
}

void APickupItem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    AddActorLocalRotation(FRotator(0.f, 180.f * DeltaTime, 0.f));
}