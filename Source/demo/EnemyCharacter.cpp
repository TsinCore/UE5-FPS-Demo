#include "EnemyCharacter.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"

AEnemyCharacter::AEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    CurrentHealth = MaxHealth;

    // 自动创建 AIController，让 AI 能移动
    AIControllerClass = AAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    // 面向移动方向
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
    bUseControllerRotationYaw = false;
}

void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;
    PatrolOrigin = GetActorLocation();
    PickNewPatrolTarget();
}

void AEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (CurrentState == EEnemyState::Dead) return;

    UE_LOG(LogTemp, Warning, TEXT("Enemy State: %d, Loc: %s, Target: %s"),
        (int32)CurrentState,
        *GetActorLocation().ToString(),
        *CurrentPatrolTarget.ToString());

    UpdateState(DeltaTime);
}

void AEnemyCharacter::UpdateState(float DeltaTime)
{
    switch (CurrentState)
    {
    case EEnemyState::Patrol: Patrol(DeltaTime); break;
    case EEnemyState::Chase:  Chase(DeltaTime);  break;
    case EEnemyState::Attack: Attack(DeltaTime); break;
    default: break;
    }
}

void AEnemyCharacter::Patrol(float DeltaTime)
{
    if (CanSeePlayer())
    {
        CurrentState = EEnemyState::Chase;
        return;
    }

    if (AAIController* AI = Cast<AAIController>(GetController()))
    {
        AI->MoveToLocation(CurrentPatrolTarget, 50.f);
    }

    if (FVector::Dist(GetActorLocation(), CurrentPatrolTarget) < 100.f)
    {
        PickNewPatrolTarget();
    }
}

void AEnemyCharacter::Chase(float DeltaTime)
{
    AActor* Player = GetPlayerActor();
    if (!Player)
    {
        CurrentState = EEnemyState::Patrol;
        return;
    }

    float Dist = FVector::Dist(GetActorLocation(), Player->GetActorLocation());

    // 放宽到 1.2 倍，抵消碰撞体和寻路误差
    if (Dist <= AttackRange * 1.2f)
    {
        CurrentState = EEnemyState::Attack;
        return;
    }

    if (Dist > DetectionRange * 1.5f)
    {
        CurrentState = EEnemyState::Patrol;
        return;
    }

    if (AAIController* AI = Cast<AAIController>(GetController()))
    {
        AI->MoveToActor(Player, AttackRange * 0.5f);
    }
}

void AEnemyCharacter::Attack(float DeltaTime)
{
    AActor* Player = GetPlayerActor();
    if (!Player)
    {
        CurrentState = EEnemyState::Patrol;
        return;
    }

    float Dist = FVector::Dist(GetActorLocation(), Player->GetActorLocation());

    // 放宽到 1.5 倍才切回 Chase，避免来回抖
    if (Dist > AttackRange * 1.5f)
    {
        CurrentState = EEnemyState::Chase;
        return;
    }

    float Now = GetWorld()->GetTimeSeconds();
    if (Now - LastAttackTime < AttackInterval) return;
    LastAttackTime = Now;

    UGameplayStatics::ApplyDamage(Player, AttackDamage, GetController(), this, nullptr);
    UE_LOG(LogTemp, Warning, TEXT("Enemy attacked player for %f damage"), AttackDamage);
}

void AEnemyCharacter::PickNewPatrolTarget()
{
    FVector Offset = FVector(
        FMath::RandRange(-500.f, 500.f),
        FMath::RandRange(-500.f, 500.f),
        0.f
    );
    CurrentPatrolTarget = PatrolOrigin + Offset;
}

bool AEnemyCharacter::CanSeePlayer() const
{
    AActor* Player = GetPlayerActor();
    if (!Player) return false;

    float Dist = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
    if (Dist > DetectionRange) return false;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    FVector Start = GetActorLocation() + FVector(0, 0, 50.f);
    FVector End = Player->GetActorLocation();

    bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
    return !bHit || Hit.GetActor() == Player;
}

AActor* AEnemyCharacter::GetPlayerActor() const
{
    return UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

float AEnemyCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
    class AController* EventInstigator, AActor* DamageCauser)
{
    Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (CurrentState == EEnemyState::Dead) return 0.f;

    CurrentHealth -= DamageAmount;
    UE_LOG(LogTemp, Warning, TEXT("Enemy took %f damage, health: %f"), DamageAmount, CurrentHealth);

    if (CurrentHealth <= 0.f)
    {
        Die();
    }
    else
    {
        CurrentState = EEnemyState::Chase;
    }

    return DamageAmount;
}

void AEnemyCharacter::Die()
{
    CurrentState = EEnemyState::Dead;
    UE_LOG(LogTemp, Warning, TEXT("Enemy died!"));

    SetLifeSpan(2.f);

    if (AAIController* AI = Cast<AAIController>(GetController()))
    {
        AI->StopMovement();
    }

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}