// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "InteractionComponent.h"
#include "InventoryComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PlayerInventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("PlayerInventoryComponent"));
	PlayerInteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("PlayerInteractionComponent"));
	ItemSearchSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ItemSearchSphere"));
	ItemSearchSphere->SetupAttachment(RootComponent);
	// Preserve the existing BP sphere's radius and local offset.
	ItemSearchSphere->InitSphereRadius(50.f);
	ItemSearchSphere->SetRelativeLocation(FVector(60.f, 0.f, -60.f));
	// InteractionComponent performs explicit overlap queries using this volume.
	ItemSearchSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ItemSearchSphere->SetGenerateOverlapEvents(false);
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	CameraBoom->TargetArmLength = 350.f;

	CameraBoom->TargetOffset = FVector(0.f, 0.f, 75.f);
	CameraBoom->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));

	CameraBoom->bUsePawnControlRotation = true;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(PlayerInteractionComponent) && IsValid(ItemSearchSphere))
		PlayerInteractionComponent->InitializeInteraction(ItemSearchSphere);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
			Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		if (MouseLookAction)
			Input->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		if (LookAction)
			Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		if (JumpAction)
		{
			Input->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
			Input->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
			Input->BindAction(JumpAction, ETriggerEvent::Canceled, this, &ACharacter::StopJumping);
		}
		if (InteractionAction)
			Input->BindAction(InteractionAction, ETriggerEvent::Started, this, &APlayerCharacter::Interact);
	}
}

void APlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				// Do not clear other contexts (for example UI or controller mappings).
				if (DefaultMappingContext && !Subsystem->HasMappingContext(DefaultMappingContext))
					Subsystem->AddMappingContext(DefaultMappingContext, 0);
				if (MouseLookMappingContext && !Subsystem->HasMappingContext(MouseLookMappingContext))
					Subsystem->AddMappingContext(MouseLookMappingContext, 0);
			}
		}
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller)
		return;

	const FVector2D Movement = Value.Get<FVector2D>();
	const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
	AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), Movement.Y);
	AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), Movement.X);
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	if (!Controller)
		return;

	const FVector2D LookAxis = Value.Get<FVector2D>();
	AddControllerYawInput(LookAxis.X);
	AddControllerPitchInput(LookAxis.Y);
}

void APlayerCharacter::Interact()
{
	if (!Controller)
		return;

	if (IsValid(PlayerInteractionComponent))
		PlayerInteractionComponent->TryInteract();
}
