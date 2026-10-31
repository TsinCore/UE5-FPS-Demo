#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemData.h"
#include "PickupItem.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class DEMO_API APickupItem : public AActor
{
    GENERATED_BODY()

public:
    APickupItem();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
    FItemData ItemData;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, Category = "Pickup")
    USphereComponent* SphereComponent;

    UPROPERTY(VisibleAnywhere, Category = "Pickup")
    UStaticMeshComponent* MeshComponent;
};