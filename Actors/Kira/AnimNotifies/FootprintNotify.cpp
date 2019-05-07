// Fill out your copyright notice in the Description page of Project Settings.

#include "FootprintNotify.h"
#include <Components/SkeletalMeshComponent.h>
#include <NameTypes.h>
#include <Engine/World.h>
#include <Engine/EngineTypes.h>
#include <DrawDebugHelpers.h>
#include "Actors/Kira/EchoesCharacter.h"
#include <Quat.h>
#include "VFX/FootprintComponent.h"

void UFootprintNotify::Trace( FHitResult& sOutHit, const FVector& vLocation, AActor* pcIgnore, UWorld* pcWorld ) const
{
	FVector vStart = vLocation;
	FVector vEnd = vLocation;

	// Make raycast appear higher
	vStart.Z += 20.0f;
	// And go lower
	vEnd.Z	 -= 100.0f;

	//Re-initialize hit info
	sOutHit = FHitResult( ForceInit );

	FCollisionQueryParams sTraceParams = FCollisionQueryParams( FName( TEXT( "FootprintTrace" ) ), true, pcIgnore );
	sTraceParams.bReturnPhysicalMaterial = true; //We need the material returned as well (see below).

	if( pcWorld )
	{
		// pcWorld->DebugDrawTraceTag = FName( TEXT( "FootprintTrace" ) );
		pcWorld->LineTraceSingleByChannel( sOutHit, vStart, vEnd, ECC_Visibility, sTraceParams );
	}
}

void UFootprintNotify::Notify( USkeletalMeshComponent* pcMeshComp, UAnimSequenceBase* pcAnimation )
{
	UAnimNotify::Notify( pcMeshComp, pcAnimation );

	FHitResult sHitResult;
	FVector vFootWorldPosition = pcMeshComp->GetBoneLocation( m_sBoneName );
	AEchoesCharacter* pcPlayer = Cast<AEchoesCharacter> ( pcMeshComp->GetOwner ( ) );

	if( pcPlayer )
	{
		// Shoot raycast from the bone to floor to find where the foot touches the floor
		Trace( sHitResult, vFootWorldPosition, pcPlayer , pcMeshComp->GetWorld() );

		TArray<UFootprintComponent*> aFootprintComponents;
		// Get the component responsible for leaving footprints
		pcPlayer->GetComponents<UFootprintComponent>( aFootprintComponents );

		if( aFootprintComponents.Num() > 0 )
		{
			// Create the footprint in the location where leg touches the ground, based on the material of the ground it touches
			aFootprintComponents[ 0 ]->LeaveFootprint( sHitResult.Location, pcPlayer->GetActorRotation() , sHitResult.PhysMaterial.Get() );
		}

		//play sound for footprint
		pcPlayer->PlayFootPrintSound ( );
	}
}
