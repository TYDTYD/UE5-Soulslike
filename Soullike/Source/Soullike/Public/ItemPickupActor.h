#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionTarget.h"
#include "ItemInstanceData.h"
#include "ItemPickupActor.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UWidgetComponent;

UCLASS()
class SOULLIKE_API AItemPickupActor : public AActor, public IInteractionTarget
{
    GENERATED_BODY()
public:
    AItemPickupActor();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item")
    TObjectPtr<USphereComponent> PickupCollision;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item")
    TObjectPtr<UStaticMeshComponent> ItemMesh;

    // Cached reference to the prompt widget component authored in the item BP.
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Transient, Category="Item")
    TObjectPtr<UWidgetComponent> InteractionWidget;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    FItemInstanceData ItemData;


    virtual bool CanInteract_Implementation(AActor* Interactor) const override;
    virtual bool Interact_Implementation(AActor* Interactor) override;
    virtual void SetInteractionFocused_Implementation(AActor* Interactor, bool bFocused) override;

protected:
    virtual void BeginPlay() override;

private:
    bool bCollected = false;
};
