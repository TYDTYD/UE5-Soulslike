// Fill out your copyright notice in the Description page of Project Settings.


#include "SoullikeGameInstance.h"
#include "Kismet/GameplayStatics.h"

void USoullikeGameInstance::OpenGame()
{
	if (MainLevel.IsNull()) 
	{
		UE_LOG(LogTemp, Error, TEXT("MainLevel is not set in SoullikeGameInstance"));
		return;
	}

	UGameplayStatics::OpenLevelBySoftObjectPtr(this, MainLevel);
}
