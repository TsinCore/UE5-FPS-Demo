#include "InventoryComponent.h"

UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UInventoryComponent::AddItem(const FItemData& Item)
{
    if (Item.Quantity <= 0) return false;

    // 检查是否已有该物品（同ID），有则叠加数量
    for (FItemData& ExistingItem : Items)
    {
        if (ExistingItem.ItemID == Item.ItemID)
        {
            ExistingItem.Quantity += Item.Quantity;
            OnInventoryUpdated.Broadcast();
            return true;
        }
    }

    // 没有该物品，检查容量是否足够
    if (Items.Num() >= MaxSlots) return false;

    Items.Add(Item);
    OnInventoryUpdated.Broadcast();
    return true;
}

bool UInventoryComponent::RemoveItem(FName ItemID, int32 Count)
{
    for (int32 i = 0; i < Items.Num(); i++)
    {
        if (Items[i].ItemID == ItemID)
        {
            Items[i].Quantity -= Count;
            if (Items[i].Quantity <= 0)
            {
                Items.RemoveAt(i);
            }
            OnInventoryUpdated.Broadcast();
            return true;
        }
    }
    return false;
}

bool UInventoryComponent::FindItem(FName ItemID, FItemData& OutItem) const
{
    for (const FItemData& Item : Items)
    {
        if (Item.ItemID == ItemID)
        {
            OutItem = Item;
            return true;
        }
    }
    return false;
}