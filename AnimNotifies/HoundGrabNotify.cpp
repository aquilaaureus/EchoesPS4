// Fill out your copyright notice in the Description page of Project Settings.

#include "HoundGrabNotify.h"
#include "../Actors/Hound/ASMeleeEnemy.h"
#include <Components/SkeletalMeshComponent.h>


void UHoundGrabNotify::Notify( USkeletalMeshComponent* pcMeshComp, UAnimSequenceBase* pcAnimation )
{
	AASMeleeEnemy* pcHound = Cast<AASMeleeEnemy>( pcMeshComp->GetOwner() );
	if ( pcHound )
	{
		if ( CheckPlayerIsGrabbed( pcHound ) )
		{
			pcHound->AttachToPlayer();
		}
		else
		{
			pcHound->ReturnToIdle();
		}
	}
}

//TODO: This code is mostly duplicated from HoundAttackNotify CheckPlayerIsHit()
bool UHoundGrabNotify::CheckPlayerIsGrabbed( AASMeleeEnemy* pcHound ) 
{
	//Get the distance between the hound and the player
	APawn* pcPlayerPawn = pcHound->GetWorld()->GetFirstPlayerController()->GetPawn();
	if ( IsValid( pcPlayerPawn ) )
	{
		FVector sPlayerLocation = pcPlayerPawn->GetActorLocation();
		FVector sHoundLocation = pcHound->GetMesh()->GetBoneLocation(FName(TEXT("root")));
		FVector sPositionVector = sPlayerLocation - sHoundLocation;

		//check if it's LTE the grabbing range
		if ( sPositionVector.Size() <= pcHound->m_fGrabRange )
		{
			//Compute the dot product between the player's forward vector and the hound's forward vector.
			float fDotProduct = FVector::DotProduct( pcPlayerPawn->GetActorForwardVector(), pcHound->GetActorForwardVector() );

			//If the dot product is GTE some cos(theta) value treat as a success
			if ( fDotProduct >= 0.5f )
			{
				UE_LOG( LogTemp, Warning, TEXT( "Hound Grabbed Player!" ) );
				return true;
			}
			else
			{
				UE_LOG( LogTemp, Warning, TEXT( "Hound Missed (Grab) Player!" ) );
			}
		}
	}

	return false;
}

