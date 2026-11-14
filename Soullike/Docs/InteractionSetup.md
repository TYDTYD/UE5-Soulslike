# Interaction setup and verification

## Runtime wiring
- PlayerCharacter creates PlayerInteractionComponent with CreateDefaultSubobject in its constructor. BP_Player inherits this component; do not add a second InteractionComponent in the Blueprint.
- PlayerInteractionComponent points to that component; its PlayerCharacter points back to the owner.
- PlayerCharacter also creates ItemSearchSphere in its constructor and calls InitializeInteraction in C++ BeginPlay. No BP initialization node or sphere name search is needed. The inherited sphere retains the former BP defaults: radius 50 cm, local offset (60, 0, -60). Adjust its position/radius in BP as needed.
- The sphere has NoCollision and no overlap events: it supplies the position/radius for explicit world overlap queries, not physics or event-based detection.
- Every 0.1 seconds, an object overlap query uses the sphere world position and scaled radius. This is a query, not Begin/EndOverlap event bookkeeping. GenerateOverlapEvents on the search sphere is not required. Target collision must support scene queries.
- NearbyItems contains distinct actors implementing InteractionTarget. CurrentTarget is the closest eligible actor. Destroyed/out-of-range targets disappear on the next query.
- MinimumForwardDot defaults to -1 (all nearby directions); set 0 for the forward hemisphere. Visibility blocking geometry prevents selection by default.
- F uses InteractionAction Started, rechecks the query and invokes Interact once. UI movement blocking suppresses interaction.

## Item setup after compiling and restarting the editor
- Create a Blueprint derived from ItemPickupActor, or deliberately reparent the existing empty Item actor Blueprint to ItemPickupActor.
- Set ItemData.DefinitionId to an ID recognized by the project's inventory UI/data lookup; Quantity must be positive.
- PlayerCharacter creates PlayerInventoryComponent as a native default subobject. BP_Player inherits it; do not add another InventoryComponent. ItemPickupActor finds this same component on the player. Inventory UI binding remains a separate setup step.
- ItemPickupActor supplies collision, a placeholder cube and a basic [F] Pick up text. Inventory addition must succeed before the actor disappears.
- Alternatively, implement InteractionTarget on another actor: CanInteract, Interact (success bool), SetInteractionFocused (show/hide custom prompt).
- OnInteractionTargetChanged exposes PreviousTarget and NewTarget for a screen-space prompt. A null NewTarget means hide the prompt.
- Existing TryAddItem Blueprint nodes may need Refresh Node after its return value changed from void to bool.

## Manual checks
1. Valid item + player InventoryComponent: approach, see one prompt, press F, verify exactly one item is added and pickup disappears.
2. Hold F with two items nearby: only one pickup per press.
3. Move between two candidates inside the sphere: closest eligible target changes.
4. Exit range, destroy target externally, or disable target collision: prompt clears within 0.1 seconds; F immediately rechecks.
5. Wall blocking Visibility: item cannot be selected through wall.
6. Invalid DefinitionId, zero quantity or missing inventory: no pickup and no item destruction.
7. Open inventory UI: F does not interact while input is blocked.
8. Spawn already inside range; item with multiple collision components: one candidate entry.

## Validation status
Source and existing BP wiring inspected. Editor build was attempted but sandbox execution remained in Build.bat lock wait. The elevated build request was declined; compilation and PIE validation remain unverified. No Blueprint assets or levels were changed.
