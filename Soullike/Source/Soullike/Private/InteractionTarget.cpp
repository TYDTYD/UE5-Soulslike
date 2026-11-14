#include "InteractionTarget.h"

bool IInteractionTarget::CanInteract_Implementation(AActor* Interactor) const 
{ 
	return IsValid(Interactor); 
}

bool IInteractionTarget::Interact_Implementation(AActor* Interactor) 
{ 
	return false; 
}

void IInteractionTarget::SetInteractionFocused_Implementation(AActor* Interactor, bool bFocused) 
{

}
