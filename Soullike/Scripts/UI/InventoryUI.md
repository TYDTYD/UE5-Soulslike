# Inventory and looting widgets

## Shared hierarchy

- `WBP_Inventory` and `WBP_InteractInventory` derive from `WBP_PopupBase` (native parent `UPopupWidgetBase`). Both fill its `Content` named slot.
- CloseButton continues to call inherited `OnClickClose`, so closing goes through `RequestClose` and the popup manager rather than `RemoveFromParent`.
- Both screens use `/Game/UI/WBP_InventoryItem` for bag and pocket cells. The looting target uses the same class.
- `WBP_InventoryItem` contains ItemButton, ItemIcon, QuantityText, and ItemNameText. ItemButton emits OnItemSelected. Empty cells default to disabled with hidden icons and empty labels. The existing ItemSize, GridPosition, and bIsRotated variables are preserved.

## Loot screen binding points

The 1920x1080 design scales through InventoryScale, matching the existing inventory. Equipment is on the left, player pockets/bag in the middle, and target storage on the right. The palette is neutral charcoal/gray with white type and restrained yellow accents.

- PlayerPocketGrid: six shared cells.
- PlayerItemsGrid: 32 shared cells arranged in eight columns.
- TargetItemsGrid: 48 shared cells arranged in eight columns.
- TargetNameText: set to the safe name or corpse display name.
- SearchStateText: search/container state.
- PlayerWeightText, PlayerCapacityText, TargetCapacityText: replace the em-dash placeholders with real values.
- TakeSelectedButton, TakeAllButton, DepositButton, PlayerSortButton: disabled until valid data and action handlers are connected. Hide or disable DepositButton for targets that do not allow deposits.

These counts describe the initial UI layout, not gameplay capacity limits. Rebuild or resize the grids from actual container capacity when connecting data. Empty cells must not be treated as real inventory entries.

## Current scope and next integration

This change supplies the screen design, shared item widgets, selection notification, and inherited close handling. It does not implement item transfer, search timers, drag/drop, inventory data refresh, or safe/corpse actor interaction. No hard-coded sample loot or statistics are presented as real data.

Use the existing InventoryComponent as the data authority. Bind cells to item instances, populate icons/counts, enable occupied ItemButtons, and subscribe to OnItemSelected in the containing screen. Transfer validation belongs in the inventory/container layer, not the shared item widget.

UIManagerComponent currently has no InteractInventory entry in EPopupType. Add a new enum value at the end (preserving serialized values) and map it in PopupClasses when implementing the gameplay opening path. Do not reuse an unrelated popup type or bypass the manager with AddToViewport.

The rebuild scripts are run through Unreal ProgrammaticToolset.execute_tool_script, which supplies execute_tool; they are not standalone Python programs. Rebuild the shared item widget before either screen if its class changes. Backups are under /Game/UI/Backups.
