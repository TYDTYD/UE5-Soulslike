#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class USphereComponent;
class APlayerCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionTargetChanged, AActor*, PreviousTarget, AActor*, NewTarget);

UCLASS(ClassGroup=(Interaction), meta=(BlueprintSpawnableComponent))
class SOULLIKE_API UInteractionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInteractionComponent();

    UFUNCTION(BlueprintCallable, Category="Interaction")
    void InitializeInteraction(USphereComponent* InSearchSphere);

    UFUNCTION(BlueprintCallable, Category="Interaction")
    void UpdateInteractionTarget();

    UFUNCTION(BlueprintCallable, Category="Interaction")
    bool TryInteract();

    UPROPERTY(BlueprintReadOnly, Transient, Category="Interaction")
    TObjectPtr<APlayerCharacter> PlayerCharacter;

    UPROPERTY(BlueprintReadOnly, Transient, Category="Interaction")
    TObjectPtr<USphereComponent> SearchSphere;

    UPROPERTY(BlueprintReadOnly, Transient, Category="Interaction")
    TArray<TObjectPtr<AActor>> NearbyItems;

    UPROPERTY(BlueprintReadOnly, Transient, Category="Interaction")
    TObjectPtr<AActor> CurrentTarget;

    UPROPERTY(BlueprintAssignable, Category="Interaction")
    FInteractionTargetChanged OnInteractionTargetChanged;

    // -1: all directions; 0: character's front hemisphere.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction", meta=(ClampMin="-1", ClampMax="1"))
    float MinimumForwardDot = -1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
    bool bRequireLineOfSight = true;

protected:
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    bool CanSelectTarget(AActor* Target) const;
    void SetCurrentTarget(AActor* Target);
    FTimerHandle SearchTimer;
    bool bInteracting = false;
};
