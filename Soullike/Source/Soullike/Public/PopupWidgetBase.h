#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PopupWidgetBase.generated.h"

class UPopupWidgetBase;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPopupCloseRequested, UPopupWidgetBase*, Popup);

UCLASS(Abstract, Blueprintable)
class SOULLIKE_API UPopupWidgetBase : public UUserWidget
{
    GENERATED_BODY()

public:    
    
    UPopupWidgetBase(const FObjectInitializer& ObjectInitializer);
    
    void OpenPopup();
    void ClosePopup();
    void FocusPopup();

    UFUNCTION(BlueprintCallable, Category="UIPopup")
    void RequestClose();
    
    UPROPERTY(BlueprintAssignable, Category="UIPopup")
    FOnPopupCloseRequested OnCloseRequested;

protected:
    
    UFUNCTION(BlueprintImplementableEvent, Category="UIPopup")
    void OnPopupOpened();
    
    UFUNCTION(BlueprintImplementableEvent, Category="UIPopup")
    void OnPopupClosed();
    
    UFUNCTION(BlueprintNativeEvent, Category="UIPopup")    
    UWidget* GetInitialFocusTarget() const;
    
    virtual UWidget* GetInitialFocusTarget_Implementation() const;
    virtual FReply NativeOnPreviewKeyDown(const FGeometry& Geometry, const FKeyEvent& KeyEvent) override;
};
