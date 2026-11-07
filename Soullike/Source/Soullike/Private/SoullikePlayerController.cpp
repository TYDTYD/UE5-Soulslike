// Fill out your copyright notice in the Description page of Project Settings.


#include "SoullikePlayerController.h"
#include "EnhancedInputComponent.h"
#include "UIManagerComponent.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"

void ASoullikePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();	
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent);
	if (Input && UIAction && UIManagerComponent)
	{
		Input->BindAction(UIAction, ETriggerEvent::Started, UIManagerComponent.Get(), &UUIManagerComponent::ToggleInventory);
	}
}

ASoullikePlayerController::ASoullikePlayerController()
{
	UIManagerComponent = CreateDefaultSubobject<UUIManagerComponent>(TEXT("UIManagerComponent"));
}

void ASoullikePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (UIMappingContext)
			{
				Subsystem->AddMappingContext(UIMappingContext, 10);
			}
		}
	}

	if (IsLocalController())
	{
		SetInputMode(FInputModeGameOnly());
		// Preserve the Blueprint's Set Input Mode Game Only "Flush Input" option.
		FlushPressedKeys();
		bShowMouseCursor = false;
	}
}

