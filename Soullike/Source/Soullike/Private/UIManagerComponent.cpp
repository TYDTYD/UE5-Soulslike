#include "UIManagerComponent.h"
#include "Blueprint/GameViewportSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"

UUIManagerComponent::UUIManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}
UPopupWidgetBase* UUIManagerComponent::OpenPopup(EPopupType Type)
{
    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC || !PC->IsLocalController())
        return nullptr;
    UPopupWidgetBase* Popup = PopupInstances.FindRef(Type);
    if (!IsValid(Popup))
    {
        TSubclassOf<UPopupWidgetBase> Class = PopupClasses.FindRef(Type);
        if (!Class && Type == EPopupType::Inventory)
            Class = InventoryWidgetClass;
        if (!Class)
        {
            UE_LOG(LogTemp, Warning, TEXT("No popup class configured for type %d"), static_cast<int32>(Type));
            return nullptr;
        }
        Popup = CreateWidget<UPopupWidgetBase>(PC, Class);
        if (!Popup)
            return nullptr;
        Popup->OnCloseRequested.AddUniqueDynamic(this, &UUIManagerComponent::ClosePopup);
        PopupInstances.Add(Type, Popup);
    }
    const bool bAlreadyOpen = OpenPopupStack.Contains(Popup);
    OpenPopupStack.Remove(Popup);
    OpenPopupStack.Add(Popup);
    if (!Popup->IsInViewport())
        Popup->AddToViewport(100);
    if (!bAlreadyOpen)
        Popup->OpenPopup();
    RefreshPopupState();
    return Popup;
}
void UUIManagerComponent::TogglePopup(EPopupType Type)
{
    UPopupWidgetBase* Popup = PopupInstances.FindRef(Type);
    if (Popup && OpenPopupStack.Contains(Popup))
        ClosePopup(Popup);
    else
        OpenPopup(Type);
}
void UUIManagerComponent::ClosePopup(UPopupWidgetBase* Popup)
{
    if (!Popup || OpenPopupStack.Remove(Popup) == 0)
        return;
    Popup->ClosePopup();
    RefreshPopupState();
}
void UUIManagerComponent::CloseTopPopup()
{
    if (!OpenPopupStack.IsEmpty())
        ClosePopup(OpenPopupStack.Last());
}
void UUIManagerComponent::CloseAllPopups()
{
    const auto ClosingPopups = OpenPopupStack;
    OpenPopupStack.Empty();
    for (UPopupWidgetBase* Popup : ClosingPopups)
        if (IsValid(Popup))
            Popup->ClosePopup();
    RefreshPopupState();
}
void UUIManagerComponent::ToggleInventory()
{
    TogglePopup(EPopupType::Inventory);
}
void UUIManagerComponent::RefreshPopupState()
{
    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC || !PC->IsLocalController())
        return;
    if (!OpenPopupStack.IsEmpty())
    {
        if (!bOwnsInputState)
        {
            bPreviousMouseCursor = PC->bShowMouseCursor;
            bOwnsInputState = true;
            PC->SetIgnoreMoveInput(true);
            PC->SetIgnoreLookInput(true);
            PC->FlushPressedKeys();
            if (APawn* Pawn = PC->GetPawn())
            {
                if (UPawnMovementComponent* Movement = Pawn->GetMovementComponent())
                    Movement->StopMovementImmediately();
                if (ACharacter* Character = Cast<ACharacter>(Pawn))
                    Character->StopJumping();
            }
        }
        for (int32 Index = 0; Index < OpenPopupStack.Num(); ++Index)
        {
            UPopupWidgetBase* Popup = OpenPopupStack[Index];
            Popup->SetIsEnabled(Index == OpenPopupStack.Num() - 1);
            if (UGameViewportSubsystem* Viewport = UGameViewportSubsystem::Get(GetWorld()))
            {
                FGameViewportWidgetSlot Slot = Viewport->GetWidgetSlot(Popup);
                Slot.ZOrder = 100 + Index;
                Viewport->SetWidgetSlot(Popup, Slot);
            }
        }
        UPopupWidgetBase* Top = OpenPopupStack.Last();
        FInputModeUIOnly InputMode;
        InputMode.SetWidgetToFocus(Top->TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = true;
        Top->FocusPopup();
    }
    else if (bOwnsInputState)
    {
        PC->SetIgnoreMoveInput(false);
        PC->SetIgnoreLookInput(false);
        // This manager is owned by the gameplay controller, whose baseline is GameOnly.
        PC->SetInputMode(FInputModeGameOnly());
        PC->FlushPressedKeys();
        PC->bShowMouseCursor = bPreviousMouseCursor;
        bOwnsInputState = false;
    }
}
void UUIManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    CloseAllPopups();
    for (const auto& Entry : PopupInstances)
    {
        if (UPopupWidgetBase* Popup = Entry.Value)
        {
            Popup->OnCloseRequested.RemoveDynamic(this, &UUIManagerComponent::ClosePopup);
            Popup->RemoveFromParent();
        }
    }
    PopupInstances.Empty();
    Super::EndPlay(EndPlayReason);
}
