#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class UNiagaraSystem;

UCLASS()
class DEMO_API AWeapon : public AActor
{
    GENERATED_BODY()

public:
    AWeapon();
    void Fire();
    void Reload();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    int32 MaxAmmo = 30;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    int32 CurrentAmmo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float FireRate = 0.15f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float Damage = 10.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float Range = 5000.f;

    UPROPERTY(EditAnywhere, Category = "Weapon")
    UNiagaraSystem* HitEffect;

protected:
    virtual void BeginPlay() override;

private:
    float LastFireTime = 0.f;
};