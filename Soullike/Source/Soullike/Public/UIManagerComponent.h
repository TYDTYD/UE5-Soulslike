#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PopupWidgetBase.h"
#include "UIManagerComponent.generated.h"

UENUM(BlueprintType)
enum class EPopupType : uint8
{
    None,
    Inventory, 
    Equipment, 
    Settings, 
    ItemDetails, 
    Confirmation
};

UCLASS(ClassGroup=(UI), meta=(BlueprintSpawnableComponent))
class SOULLIKE_API UUIManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UUIManagerComponent();
    
    UFUNCTION(BlueprintCallable, Category="UI|Popup")
    UPopupWidgetBase* OpenPopup(EPopupType Type);
    
    UFUNCTION(BlueprintCallable, Category="UI|Popup")
    void TogglePopup(EPopupType Type);
    
    UFUNCTION(BlueprintCallable, Category="UI|Popup")
    void ClosePopup(UPopupWidgetBase* Popup);
    
    UFUNCTION(BlueprintCallable, Category="UI|Popup")
    void CloseTopPopup();
    
    UFUNCTION(BlueprintCallable, Category="UI|Popup")
    void CloseAllPopups();
    
    UFUNCTION(BlueprintCallable, Category="UI|Popup")
    void ToggleInventory();
    
    UFUNCTION(BlueprintPure, Category="UI|Popup")
    bool HasOpenPopups() const { return !OpenPopupStack.IsEmpty(); }

protected:
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    
    UPROPERTY(EditDefaultsOnly, Category="UI")
    TMap<EPopupType, TSubclassOf<UPopupWidgetBase>> PopupClasses;
    // Preserves the existing BP inventory assignment. PopupClasses takes priority.
    
    UPROPERTY(EditDefaultsOnly, Category="UI")
    TSubclassOf<UPopupWidgetBase> InventoryWidgetClass;

private:
    void RefreshPopupState();
    UPROPERTY(Transient)
    TMap<EPopupType, TObjectPtr<UPopupWidgetBase>> PopupInstances;
    UPROPERTY(Transient)
    TArray<TObjectPtr<UPopupWidgetBase>> OpenPopupStack;
    bool bOwnsInputState = false;
    bool bPreviousMouseCursor = false;
};
