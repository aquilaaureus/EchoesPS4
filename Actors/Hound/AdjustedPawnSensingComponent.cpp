// Fill out your copyright notice in the Description page of Project Settings.

#include "AdjustedPawnSensingComponent.h"
#include "GameFramework/Pawn.h"
#include "Components/PawnNoiseEmitterComponent.h"

void UAdjustedPawnSensingComponent::SensePawn( APawn & Pawn )
{
	// Visibility checks
	bool bHasSeenPawn = false;
	bool bHasFailedLineOfSightCheck = false;
	if ( bSeePawns && ShouldCheckVisibilityOf( &Pawn ) )
	{
		if ( CouldSeePawn( &Pawn, true ) )
		{
			BroadcastOnSeePawn( Pawn );
			bHasSeenPawn = true;
		}
	}

	// Sound checks
	// No need to 'hear' something if you've already seen it!
	if ( bHasSeenPawn )
	{
		return;
	}

	// Might not be able to hear or react to the sound at all...
	if ( !bHearNoises || !OnHearNoise.IsBound() )
	{
		return;
	}

	const UPawnNoiseEmitterComponent* NoiseEmitterComponent = Pawn.GetPawnNoiseEmitterComponent();
	if ( NoiseEmitterComponent && ShouldCheckAudibilityOf( &Pawn ) )
	{
		// ToDo: This should all still be refactored more significantly.  There's no reason that we should have to
		// explicitly check "local" and "remote" (i.e. Pawn-emitted and other-source-emitted) sounds separately here.
		// The noise emitter should handle all of those details for us so the sensing component doesn't need to know about
		// them at all!
		if ( IsNoiseRelevant( Pawn, *NoiseEmitterComponent, true ) && CanHear( Pawn.GetActorLocation(), NoiseEmitterComponent->GetLastNoiseVolume( true ), bHasFailedLineOfSightCheck ) )
		{
			BroadcastOnHearLocalNoise( Pawn, Pawn.GetActorLocation(), NoiseEmitterComponent->GetLastNoiseVolume( true ) );
		}
		else if ( IsNoiseRelevant( Pawn, *NoiseEmitterComponent, false ) && CanHear( NoiseEmitterComponent->LastRemoteNoisePosition, NoiseEmitterComponent->GetLastNoiseVolume( false ), false ) )
		{
			BroadcastOnHearRemoteNoise( Pawn, NoiseEmitterComponent->LastRemoteNoisePosition, NoiseEmitterComponent->GetLastNoiseVolume( false ) );
		}
	}

}
