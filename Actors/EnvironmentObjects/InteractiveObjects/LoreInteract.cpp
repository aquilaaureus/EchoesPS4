// Fill out your copyright notice in the Description page of Project Settings.

// Header include
#include "LoreInteract.h"

// Other includes
#include "EchoesGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "UISystems/UIEventDispatcher.h"

//////////////////////////////////////////////////////////////////////////
// Callback function for when the player interacts with the object
//////////////////////////////////////////////////////////////////////////
void ALoreInteract::CBInteract()
{
	// Fire the ability gained event
	AEchoesGameMode* pcEchoesGameMode = Cast<AEchoesGameMode>( UGameplayStatics::GetGameMode( this ) );
	if( pcEchoesGameMode )
	{
		AUIEventDispatcher* pcUIEventDispatcher = pcEchoesGameMode->GetUIEventDispatcher();
		if( pcUIEventDispatcher )
		{
			pcUIEventDispatcher->OnCodexUpdated.Broadcast( m_strCodexEntryCategory, m_strCodexEntryName );
		}
	}

	// Reset input
	// If the object should only activate once, remove bindings.
	if (!m_bMultipleActivation)
	{
		// Does the Interactable have a box collider
		if (!m_bHasBoxCollider)
		{
			// If not, remove only the sphere colliders and reset input
			RemoveSphereDynamics();
		}
	}

	// Always reset input 
	ResetInput();
}