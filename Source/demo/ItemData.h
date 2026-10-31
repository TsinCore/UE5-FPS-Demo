#pragma once

#include "CoreMinimal.h"
#include "ItemData.generated.h"

// 物品类型枚举
UENUM(BlueprintType)
enum class EItemType : uint8
{
    Consumable, // 消耗品（如医疗包）
    Ammo,       // 弹药
    Weapon,     // 武器
    Quest       // 任务物品
};

// 物品数据结构（不继承 UObject，纯数据）
USTRUCT(BlueprintType)
struct FItemData
{
    GENERATED_BODY()

    // 物品唯一ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FName ItemID;

    // 物品名称
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FText ItemName;

    // 物品类型
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    EItemType ItemType;

    // 数量
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 Quantity;

    // 图标（UI用）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    UTexture2D* Icon;

    // 默认构造函数
    FItemData()
        : ItemID(NAME_None)
        , ItemName(FText::GetEmpty())
        , ItemType(EItemType::Consumable)
        , Quantity(0)
        , Icon(nullptr)
    {
    }

    // 带参构造函数
    FItemData(FName InID, FText InName, EItemType InType, int32 InQuantity, UTexture2D* InIcon = nullptr)
        : ItemID(InID)
        , ItemName(InName)
        , ItemType(InType)
        , Quantity(InQuantity)
        , Icon(InIcon)
    {
    }
};