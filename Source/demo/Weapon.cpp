#include "Weapon.h"
#include "FPSCharacter.h"
#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Kismet/GameplayStatics.h"

AWeapon::AWeapon()
{
    PrimaryActorTick.bCanEverTick = false;
    CurrentAmmo = MaxAmmo;
}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();
    CurrentAmmo = MaxAmmo;
}

void AWeapon::Fire()
{
    AFPSCharacter* OwnerCharacter = Cast<AFPSCharacter>(GetOwner());
    if (!OwnerCharacter) return;

    float Now = GetWorld()->GetTimeSeconds();
    if (Now - LastFireTime < FireRate) return;
    LastFireTime = Now;

    if (CurrentAmmo <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No ammo!"));
        return;
    }
    CurrentAmmo--;

    FVector CameraLocation;
    FRotator CameraRotation;
    OwnerCharacter->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);

    FVector Start = CameraLocation;
    FVector End = Start + CameraRotation.Vector() * Range;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(OwnerCharacter);
    Params.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

    if (bHit)
    {
        UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *Hit.GetActor()->GetName());

        UGameplayStatics::ApplyDamage(Hit.GetActor(), Damage, OwnerCharacter->GetController(), this, nullptr);

        UE_LOG(LogTemp, Warning, TEXT("Hit: %s, Ammo: %d"), *Hit.GetActor()->GetName(), CurrentAmmo);

        if (HitEffect)
        {
            double SpawnStartTime = FPlatformTime::Seconds();

            UNiagaraFunctionLibrary::SpawnSystemAttached(
                HitEffect,
                Hit.GetActor()->GetRootComponent(),
                NAME_None,
                Hit.ImpactPoint,
                Hit.ImpactNormal.Rotation(),
                EAttachLocation::KeepWorldPosition,
                true,
                true,
                ENCPoolMethod::AutoRelease,
                true
            );

            double SpawnEndTime = FPlatformTime::Seconds();
            double SpawnElapsedMs = (SpawnEndTime - SpawnStartTime) * 1000.0;
            UE_LOG(LogTemp, Warning, TEXT("[Perf] Spawn Cost: %f ms"), SpawnElapsedMs);
        }
    }
}

void AWeapon::Reload()
{
    CurrentAmmo = MaxAmmo;
    UE_LOG(LogTemp, Warning, TEXT("Reloaded! Ammo: %d"), CurrentAmmo);
}