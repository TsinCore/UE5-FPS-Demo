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

    //FColor LineColor = bHit ? FColor::Red : FColor::Green;
    //FVector LineEnd = bHit ? Hit.ImpactPoint : End;
    //DrawDebugLine(GetWorld(), Start, LineEnd, LineColor, false, 2.f, 0, 1.f);

    if (bHit)
    {
        UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *Hit.GetActor()->GetName());

        UGameplayStatics::ApplyDamage(Hit.GetActor(), Damage, OwnerCharacter->GetController(), this, nullptr);

        UE_LOG(LogTemp, Warning, TEXT("Hit: %s, Ammo: %d"), *Hit.GetActor()->GetName(), CurrentAmmo);

        if (HitEffect)
        {
            UNiagaraFunctionLibrary::SpawnSystemAttached(
                HitEffect,
                Hit.GetActor()->GetRootComponent(),
                NAME_None,
                Hit.ImpactPoint,
                Hit.ImpactNormal.Rotation(),
                EAttachLocation::KeepWorldPosition,
                true
            );
        }
    }
}

void AWeapon::Reload()
{
    CurrentAmmo = MaxAmmo;
    UE_LOG(LogTemp, Warning, TEXT("Reloaded! Ammo: %d"), CurrentAmmo);
}