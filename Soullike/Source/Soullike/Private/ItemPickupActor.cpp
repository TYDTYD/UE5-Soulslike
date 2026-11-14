#include "ItemPickupActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/StaticMesh.h"
#include "InventoryComponent.h"
#include "UObject/ConstructorHelpers.h"

AItemPickupActor::AItemPickupActor()
{
    PrimaryActorTick.bCanEverTick = false;
    PickupCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollision"));
    SetRootComponent(PickupCollision);
    PickupCollision->InitSphereRadius(25.f);
    PickupCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    PickupCollision->SetCollisionObjectType(ECC_WorldDynamic);
    PickupCollision->SetCollisionResponseToAllChannels(ECR_Overlap);
    PickupCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    PickupCollision->SetGenerateOverlapEvents(true);
    ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
    ItemMesh->SetupAttachment(RootComponent);
    ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (Mesh.Succeeded())
        ItemMesh->SetStaticMesh(Mesh.Object);
    ItemMesh->SetRelativeScale3D(FVector(0.3f));
}

void AItemPickupActor::BeginPlay()
{
    // BP construction has completed; use its existing prompt component.
    InteractionWidget = FindComponentByClass<UWidgetComponent>();
    if (IsValid(InteractionWidget))
    {
        InteractionWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        InteractionWidget->SetVisibility(false);
        // Ensure BP BeginPlay can obtain the widget instance to set its text.
        InteractionWidget->InitWidget();
    }
    Super::BeginPlay();
}

bool AItemPickupActor::CanInteract_Implementation(AActor* Interactor) const
{
    return !bCollected && IsValid(Interactor) && ItemData.DefinitionId != NAME_None &&
        ItemData.Quantity > 0 && Interactor->FindComponentByClass<UInventoryComponent>() != nullptr;
}

bool AItemPickupActor::Interact_Implementation(AActor* Interactor)
{
    if (!CanInteract_Implementation(Interactor))
        return false;
    UInventoryComponent* Inventory = Interactor->FindComponentByClass<UInventoryComponent>();
    // Claim before broadcasting an inventory change to prevent reentrant pickup.
    bCollected = true;
    FItemInstanceData AcquiredItem = ItemData;
    AcquiredItem.InstanceId = FGuid::NewGuid();
    if (!Inventory->TryAddItem(AcquiredItem))
    {
        bCollected = false;
        return false;
    }
    if (IsValid(InteractionWidget))
        InteractionWidget->SetVisibility(false);
    SetActorEnableCollision(false);
    Destroy();
    return true;
}

void AItemPickupActor::SetInteractionFocused_Implementation(AActor* Interactor, bool bFocused)
{
    if (IsValid(InteractionWidget))
    {
        InteractionWidget->SetVisibility(bFocused && !bCollected);
    }
}
