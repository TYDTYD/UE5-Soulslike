#include "InteractionComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/PlayerController.h"
#include "InteractionTarget.h"
#include "PlayerCharacter.h"
#include "TimerManager.h"

UInteractionComponent::UInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UInteractionComponent::InitializeInteraction(USphereComponent* InSearchSphere)
{
    if (!IsValid(InSearchSphere) || InSearchSphere->GetOwner() != GetOwner())
        return;

    PlayerCharacter = Cast<APlayerCharacter>(GetOwner());

    if (!PlayerCharacter)
        return;

    SearchSphere = InSearchSphere;
    GetWorld()->GetTimerManager().ClearTimer(SearchTimer);
    GetWorld()->GetTimerManager().SetTimer(SearchTimer, this, &UInteractionComponent::UpdateInteractionTarget, 0.1f, true);
    UpdateInteractionTarget();
}
bool UInteractionComponent::CanSelectTarget(AActor* Target) const
{
    if (!IsValid(Target) || !IsValid(PlayerCharacter) || Target == PlayerCharacter ||
        !Target->Implements<UInteractionTarget>() ||
        !IInteractionTarget::Execute_CanInteract(Target, PlayerCharacter))
        return false;

    const FVector Direction = (Target->GetActorLocation() - PlayerCharacter->GetActorLocation()).GetSafeNormal2D();

    if (!Direction.IsNearlyZero() &&
        FVector::DotProduct(PlayerCharacter->GetActorForwardVector(), Direction) < MinimumForwardDot)
        return false;

    if (bRequireLineOfSight)
    {
        FHitResult Hit;
        FCollisionQueryParams Params(SCENE_QUERY_STAT(InteractionVisibility), false, PlayerCharacter);
        if (GetWorld()->LineTraceSingleByChannel(Hit, PlayerCharacter->GetPawnViewLocation(),
            Target->GetActorLocation(), ECC_Visibility, Params) && Hit.GetActor() != Target)
            return false;
    }

    return true;
}
void UInteractionComponent::UpdateInteractionTarget()
{
    NearbyItems.Reset();
    AActor* BestTarget = nullptr;
    double BestDistance = TNumericLimits<double>::Max();
    if (IsValid(SearchSphere) && IsValid(PlayerCharacter))
    {
        // Query the authored sphere volume. No dependency on overlap event order,
        // initial overlaps or the number of collision components on an item.
        TArray<FOverlapResult> Overlaps;
        FCollisionObjectQueryParams Objects(FCollisionObjectQueryParams::AllObjects);
        FCollisionQueryParams Params(SCENE_QUERY_STAT(InteractionSearch), false, PlayerCharacter);

        GetWorld()->OverlapMultiByObjectType(Overlaps, SearchSphere->GetComponentLocation(),
            FQuat::Identity, Objects, FCollisionShape::MakeSphere(SearchSphere->GetScaledSphereRadius()), Params);

        for (const FOverlapResult& Overlap : Overlaps)
        {
            AActor* Candidate = Overlap.GetActor();
            if (IsValid(Candidate) && Candidate != PlayerCharacter && Candidate->Implements<UInteractionTarget>())
                NearbyItems.AddUnique(Candidate);
        }

        for (AActor* Candidate : NearbyItems)
        {
            if (!CanSelectTarget(Candidate))
                continue;
            const double Distance = FVector::DistSquared(PlayerCharacter->GetActorLocation(), Candidate->GetActorLocation());
            if (Distance < BestDistance || (Distance == BestDistance && Candidate == CurrentTarget))
            {
                BestDistance = Distance;
                BestTarget = Candidate;
            }
        }

    }
    SetCurrentTarget(BestTarget);
}
void UInteractionComponent::SetCurrentTarget(AActor* Target)
{
    if (CurrentTarget == Target)
        return;
    AActor* Previous = CurrentTarget;
    CurrentTarget = Target;

    if (IsValid(Previous))
        IInteractionTarget::Execute_SetInteractionFocused(Previous, PlayerCharacter, false);

    if (IsValid(CurrentTarget))
        IInteractionTarget::Execute_SetInteractionFocused(CurrentTarget, PlayerCharacter, true);

    OnInteractionTargetChanged.Broadcast(IsValid(Previous) ? Previous : nullptr, CurrentTarget);
}
bool UInteractionComponent::TryInteract()
{
    if (bInteracting || !IsValid(PlayerCharacter))
        return false;

    const APlayerController* PC = Cast<APlayerController>(PlayerCharacter->GetController());
    if (!PC || PC->IsMoveInputIgnored())
        return false;

    TGuardValue<bool> Guard(bInteracting, true);
    UpdateInteractionTarget();
    AActor* Target = CurrentTarget;

    if (!CanSelectTarget(Target))
        return false;

    const bool bSuccess = IInteractionTarget::Execute_Interact(Target, PlayerCharacter);
    UpdateInteractionTarget();
    return bSuccess;
}
void UInteractionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    GetWorld()->GetTimerManager().ClearTimer(SearchTimer);
    SetCurrentTarget(nullptr);
    NearbyItems.Reset();
    SearchSphere = nullptr;
    PlayerCharacter = nullptr;
    Super::EndPlay(EndPlayReason);
}
