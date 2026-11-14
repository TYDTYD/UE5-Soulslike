#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionTarget.generated.h"

UINTERFACE(BlueprintType)
class SOULLIKE_API UInteractionTarget : public UInterface
{
    GENERATED_BODY()
};
class SOULLIKE_API IInteractionTarget
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
    bool CanInteract(AActor* Interactor) const;

    virtual bool CanInteract_Implementation(AActor* Interactor) const;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
    bool Interact(AActor* Interactor);

    virtual bool Interact_Implementation(AActor* Interactor);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
    void SetInteractionFocused(AActor* Interactor, bool bFocused);

    virtual void SetInteractionFocused_Implementation(AActor* Interactor, bool bFocused);
};
