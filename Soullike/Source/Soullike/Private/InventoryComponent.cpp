// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "ItemInstanceData.h"

// Sets default values
UInventoryComponent::UInventoryComponent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryComponentTick.bCanEverTick = false;

}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

// todo : 반환 결과 리턴하기 -> 로직 성공했는 지 아닌 지
bool UInventoryComponent::TryAddItem(const FItemInstanceData& Item)
{
	if (Item.Quantity <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Item quantity is zero or less. Can't add Item!"));
		return false;
	}	

	if (Item.DefinitionId == NAME_None)
	{
		UE_LOG(LogTemp, Warning, TEXT("Item DefinitionId is None. Can't add Item!"));
		return false;
	}

	// TODO : 아이템 검증 로직 추가	-> 서버쪽으로 이전 예정
	FItemInstanceData NewItem = Item;
	if (!NewItem.InstanceId.IsValid())
		NewItem.InstanceId = FGuid::NewGuid();

	if (Items.ContainsByPredicate([&NewItem](const FItemInstanceData& Existing)
		{ 
			return Existing.InstanceId == NewItem.InstanceId; 
		}))
		return false;

	Items.Add(NewItem);
	OnChangeItem.Broadcast();
	return true;
}

// todo : 반환 결과 리턴하기 -> 로직 성공했는 지 아닌 지
void UInventoryComponent::TryUseItem(const FItemInstanceData& Item)
{
	if (Item.Quantity <= 0) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Item quantity is zero or less. Can't use Item!"));
		return;
	}

	if (Item.DefinitionId == NAME_None)
	{
		UE_LOG(LogTemp, Warning, TEXT("Item DefinitionId is None. Can't use Item!"));
		return;
	}
		
	bool result = false;

	// TODO : 아이템 검증 로직 추가 -> 서버쪽으로 이전 예정
	for (FItemInstanceData& ItemInstance : Items)
	{
		if (ItemInstance.DefinitionId == Item.DefinitionId && ItemInstance.InstanceId == Item.InstanceId)
		{
			if (ItemInstance.Quantity >= Item.Quantity)
			{
				ItemInstance.Quantity -= Item.Quantity;
				if (ItemInstance.Quantity <= 0)
				{
					Items.Remove(ItemInstance);
				}
				result = true;
			}
			break;
		}
	}

	if (result) 
	{
		OnChangeItem.Broadcast();
	}
}

// todo : 반환 결과 리턴하기 -> 로직 성공했는 지 아닌 지
void UInventoryComponent::TryRemoveItem(const FItemInstanceData& Item)
{	
	if (Item.DefinitionId == NAME_None)
	{
		UE_LOG(LogTemp, Warning, TEXT("Item DefinitionId is None. Can't remove Item!"));
		return;
	}

	// TODO : 아이템 검증 로직 추가 -> 서버쪽으로 이전 예정
	if (Items.Remove(Item))
	{
		OnChangeItem.Broadcast();
	}
}

