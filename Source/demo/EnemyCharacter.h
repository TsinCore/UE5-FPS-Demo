#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
    Patrol,
    Chase,
    Attack,
    Dead
};

UCLASS()
class DEMO_API AEnemyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyCharacter();

    virtual void Tick(float DeltaTime) override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator, AActor* DamageCauser) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    float MaxHealth = 50.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
    float CurrentHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    float DetectionRange = 1500.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    float AttackRange = 250.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    float AttackDamage = 10.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    float AttackInterval = 1.5f;

protected:
    virtual void BeginPlay() override;

private:
    EEnemyState CurrentState = EEnemyState::Patrol;
    float LastAttackTime = 0.f;
    FVector PatrolOrigin;
    FVector CurrentPatrolTarget;

    void UpdateState(float DeltaTime);
    void Patrol(float DeltaTime);
    void Chase(float DeltaTime);
    void Attack(float DeltaTime);
    void Die();

    void PickNewPatrolTarget();
    bool CanSeePlayer() const;
    AActor* GetPlayerActor() const;
};