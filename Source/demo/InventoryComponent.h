#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemData.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DEMO_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInventoryComponent();

    // 添加物品
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool AddItem(const FItemData& Item);

    // 移除物品
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool RemoveItem(FName ItemID, int32 Count = 1);

    // 查找物品
    UFUNCTION(BlueprintPure, Category = "Inventory")
    bool FindItem(FName ItemID, FItemData& OutItem) const;

    // 获取所有物品
    UFUNCTION(BlueprintPure, Category = "Inventory")
    const TArray<FItemData>& GetAllItems() const { return Items; }

    // 背包更新事件（通知UI刷新）
    UPROPERTY(BlueprintAssignable, Category = "Inventory")
    FOnInventoryUpdated OnInventoryUpdated;

    // 背包最大容量（格子数）
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
    int32 MaxSlots = 20;

protected:
    // 存储物品的数组
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TArray<FItemData> Items;
};