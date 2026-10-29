// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

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

void UInventoryComponent::TryAddItem(const FItemInstanceData& Item)
{

}

void UInventoryComponent::TryRemoveItem(const FItemInstanceData& Item)
{

}

