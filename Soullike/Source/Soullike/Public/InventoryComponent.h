// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

struct FItemInstanceData;

UCLASS(ClassGroup = (Inventory), meta = (BlueprintSpawnableComponent))
class SOULLIKE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UInventoryComponent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void TryAddItem(const FItemInstanceData& Item);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void TryRemoveItem(const FItemInstanceData& Item);
};
