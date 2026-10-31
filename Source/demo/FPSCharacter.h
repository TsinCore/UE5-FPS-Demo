#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Weapon.h" 
#include "FPSCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UUserWidget;
class UInventoryComponent;
struct FInputActionValue;

UCLASS()
class DEMO_API AFPSCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AFPSCharacter();

    UFUNCTION(BlueprintPure, Category = "Inventory")
    UInventoryComponent* GetInventory() const { return InventoryComponent; }

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool UseItem(FName ItemID);

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator, AActor* DamageCauser) override;
    virtual void Tick(float DeltaTime) override;

    void ShowInteractPrompt(bool bShow);
    void MoveForward(const FInputActionValue& Value);
    void MoveRight(const FInputActionValue& Value);
    void Turn(const FInputActionValue& Value);
    void LookUp(const FInputActionValue& Value);
    void Shoot();
    void Reload();
    void Interact();
    void ToggleInventory();
    

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* MoveForwardAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* MoveRightAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* TurnAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* LookUpAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* ShootAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* JumpAction;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    AWeapon* CurrentWeapon;

    UPROPERTY(EditAnywhere, Category = "Weapon")
    TSubclassOf<AWeapon> WeaponClass;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* ReloadAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    float MaxHealth = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
    float CurrentHealth;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> HUDWidgetClass;

    UPROPERTY()
    UUserWidget* HUDWidget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    UInventoryComponent* InventoryComponent;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* InteractAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* InventoryAction;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> InventoryWidgetClass;

    UPROPERTY()
    UUserWidget* InventoryWidget;

    UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
    USkeletalMeshComponent* FirstPersonMesh;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> InteractPromptClass;

    UPROPERTY()
    UUserWidget* InteractPromptWidget;

};