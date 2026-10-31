#include "FPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "DrawDebugHelpers.h"
#include "Weapon.h"
#include "Blueprint/UserWidget.h"
#include "InventoryComponent.h"
#include "PickupItem.h"

AFPSCharacter::AFPSCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    bUseControllerRotationYaw = true;

    // 1. 先创建第一人称手臂，挂到 Capsule
    FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
    FirstPersonMesh->SetOnlyOwnerSee(true);
    FirstPersonMesh->SetupAttachment(GetCapsuleComponent());
    FirstPersonMesh->SetRelativeLocation(FVector(0.f, 0.f, 64.f));
    FirstPersonMesh->bCastDynamicShadow = false;
    FirstPersonMesh->CastShadow = false;

    // 2. 再创建摄像机，挂到手臂的 head socket
    UCameraComponent* Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(FirstPersonMesh, TEXT("Head"));
    Camera->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
    Camera->SetRelativeRotation(FRotator(0.f, 0.f, -90.f));
    Camera->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));
    Camera->bUsePawnControlRotation = true;

    // 3. 背包组件
    InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

    // 4. 拥有者（自己）看不到第三人称身体
    GetMesh()->SetOwnerNoSee(true);
}

void AFPSCharacter::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;

    // 增强输入绑定（已有代码保留）
    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    // 生成武器并附加到角色
    if (WeaponClass)
    {
        FActorSpawnParameters Params;
        Params.Owner = this;
        CurrentWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, GetActorTransform(), Params);
    }

    if (HUDWidgetClass)
    {
        HUDWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
        if (HUDWidget) HUDWidget->AddToViewport();
    }

    if (InteractPromptClass)
    {
        InteractPromptWidget = CreateWidget<UUserWidget>(GetWorld(), InteractPromptClass);
        if (InteractPromptWidget)
        {
            InteractPromptWidget->AddToViewport();
            InteractPromptWidget->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}

void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EIC->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &AFPSCharacter::MoveForward);
        EIC->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &AFPSCharacter::MoveRight);
        EIC->BindAction(TurnAction, ETriggerEvent::Triggered, this, &AFPSCharacter::Turn);
        EIC->BindAction(LookUpAction, ETriggerEvent::Triggered, this, &AFPSCharacter::LookUp);
        EIC->BindAction(ShootAction, ETriggerEvent::Started, this, &AFPSCharacter::Shoot);
        EIC->BindAction(ShootAction, ETriggerEvent::Triggered, this, &AFPSCharacter::Shoot);
        EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
        EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
        EIC->BindAction(ReloadAction, ETriggerEvent::Started, this, &AFPSCharacter::Reload);
        EIC->BindAction(InteractAction, ETriggerEvent::Started, this, &AFPSCharacter::Interact);
        EIC->BindAction(InventoryAction, ETriggerEvent::Started, this, &AFPSCharacter::ToggleInventory);
    }
}

void AFPSCharacter::MoveForward(const FInputActionValue& Value)
{
    float Axis = Value.Get<float>();
    if (Controller && Axis != 0.f)
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Axis);
    }
}

void AFPSCharacter::MoveRight(const FInputActionValue& Value)
{
    float Axis = Value.Get<float>();
    if (Controller && Axis != 0.f)
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Axis);
    }
}

void AFPSCharacter::Turn(const FInputActionValue& Value)
{
    AddControllerYawInput(Value.Get<float>());
}

void AFPSCharacter::LookUp(const FInputActionValue& Value)
{
    AddControllerPitchInput(Value.Get<float>());
}

void AFPSCharacter::Shoot()
{
    // 背包打开时不射击
    if (InventoryWidget && InventoryWidget->IsInViewport())
    {
        return;
    }

    if (CurrentWeapon)
    {
        CurrentWeapon->Fire();
    }
}

void AFPSCharacter::Reload()
{
    if (CurrentWeapon)
    {
        CurrentWeapon->Reload();
    }
}

float AFPSCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
    class AController* EventInstigator, AActor* DamageCauser)
{
    Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (CurrentHealth <= 0.f) return 0.f;  // 已死，不再扣血

    CurrentHealth -= DamageAmount;

    if (CurrentHealth <= 0.f)
    {
        CurrentHealth = 0.f;
        UE_LOG(LogTemp, Warning, TEXT("Player died!"));

        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            DisableInput(PC);
        }
        GetCharacterMovement()->DisableMovement();
    }

    return DamageAmount;
}

void AFPSCharacter::Interact()
{
    // 从摄像机位置向前发射射线，找可拾取物
    FVector CameraLocation;
    FRotator CameraRotation;
    if (Controller)
    {
        Controller->GetPlayerViewPoint(CameraLocation, CameraRotation);
    }

    FVector Start = CameraLocation;
    FVector End = Start + CameraRotation.Vector() * 300.f;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

    if (bHit)
    {
        APickupItem* Pickup = Cast<APickupItem>(Hit.GetActor());
        if (Pickup && InventoryComponent)
        {
            if (InventoryComponent->AddItem(Pickup->ItemData))
            {
                UE_LOG(LogTemp, Warning, TEXT("Picked up: %s"), *Pickup->ItemData.ItemName.ToString());
                Pickup->Destroy();
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Inventory full!"));
            }
        }
    }
}

void AFPSCharacter::ToggleInventory()
{
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC) return;

    if (!InventoryWidget)
    {
        if (InventoryWidgetClass)
        {
            InventoryWidget = CreateWidget<UUserWidget>(GetWorld(), InventoryWidgetClass);
            InventoryWidget->AddToViewport();
            PC->SetShowMouseCursor(true);
            PC->SetInputMode(FInputModeGameAndUI());
        }
    }
    else
    {
        if (InventoryWidget->IsInViewport())
        {
            InventoryWidget->RemoveFromParent();
            PC->SetShowMouseCursor(false);
            PC->SetInputMode(FInputModeGameOnly());
        }
        else
        {
            InventoryWidget->AddToViewport();
            PC->SetShowMouseCursor(true);
            PC->SetInputMode(FInputModeGameAndUI());
        }
    }
}

bool AFPSCharacter::UseItem(FName ItemID)
{
    if (!InventoryComponent) return false;

    FItemData Item;
    if (!InventoryComponent->FindItem(ItemID, Item)) return false;

    // 医疗包：回血
    if (ItemID == FName("HealthPack"))
    {
        if (CurrentHealth >= MaxHealth) return false;
        CurrentHealth = FMath::Min(CurrentHealth + 30.f, MaxHealth);
        InventoryComponent->RemoveItem(ItemID, 1);
        UE_LOG(LogTemp, Warning, TEXT("Used HealthPack, Health: %f"), CurrentHealth);
        return true;
    }

    return false;
}

void AFPSCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // FirstPersonMesh Pitch 同步代码
    if (FirstPersonMesh && Controller)
    {
        FRotator ControlRot = Controller->GetControlRotation();
        FRotator MeshRot = FirstPersonMesh->GetRelativeRotation();
        MeshRot.Pitch = ControlRot.Pitch;
        FirstPersonMesh->SetRelativeRotation(MeshRot);
    }

    // 射线检测前方是否有可拾取物
    if (Controller)
    {
        FVector CameraLocation;
        FRotator CameraRotation;
        Controller->GetPlayerViewPoint(CameraLocation, CameraRotation);
        FVector End = CameraLocation + CameraRotation.Vector() * 300.f;

        FHitResult Hit;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this);

        bool bHit = GetWorld()->LineTraceSingleByChannel(
            Hit, CameraLocation, End, ECC_Visibility, Params);

        APickupItem* Pickup = bHit ? Cast<APickupItem>(Hit.GetActor()) : nullptr;
        ShowInteractPrompt(Pickup != nullptr);
    }
}

void AFPSCharacter::ShowInteractPrompt(bool bShow)
{
    if (InteractPromptWidget)
    {
        InteractPromptWidget->SetVisibility(
            bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
}