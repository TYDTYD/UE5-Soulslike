// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SoullikeGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SOULLIKE_API USoullikeGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Game")
	void OpenGame();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Game|Maps")
	TSoftObjectPtr<UWorld> MainLevel;

};
