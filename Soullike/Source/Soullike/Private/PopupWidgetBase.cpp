#include "PopupWidgetBase.h"
#include "InputCoreTypes.h"

UPopupWidgetBase::UPopupWidgetBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    SetIsFocusable(true);
}

void UPopupWidgetBase::OpenPopup()
{
    SetVisibility(ESlateVisibility::Visible);
    OnPopupOpened();
}

void UPopupWidgetBase::ClosePopup()
{
    SetVisibility(ESlateVisibility::Collapsed);
    OnPopupClosed();
}

void UPopupWidgetBase::RequestClose()
{
    OnCloseRequested.Broadcast(this);
}

UWidget* UPopupWidgetBase::GetInitialFocusTarget_Implementation() const
{
    return nullptr;
}

void UPopupWidgetBase::FocusPopup()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        UWidget* Target = GetInitialFocusTarget();
        (Target ? Target : this)->SetUserFocus(PC);
    }
}

FReply UPopupWidgetBase::NativeOnPreviewKeyDown(const FGeometry& Geometry, const FKeyEvent& KeyEvent)
{
    if (KeyEvent.GetKey() == EKeys::Escape || KeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Right)
    {
        if (!KeyEvent.IsRepeat())
            RequestClose();
        return FReply::Handled();
    }
    return Super::NativeOnPreviewKeyDown(Geometry, KeyEvent);
}
