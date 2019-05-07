// Fill out your copyright notice in the Description page of Project Settings.

#include "HoundAttackNotify.h"
#include "../Actors/Hound/ASMeleeEnemy.h"
#include <Components/SkeletalMeshComponent.h>


void UHoundAttackNotify::Notify( USkeletalMeshComponent* pcMeshComp, UAnimSequenceBase* pcAnimation )
{
	AASMeleeEnemy* pcHound = Cast<AASMeleeEnemy>( pcMeshComp->GetOwner() );
	if( pcHound )
	{
		if (CheckPlayerIsHit( pcHound )) 
		{
			pcHound->DealDamageToPlayer();
		}
		else 
		{
			pcHound->ReturnToIdle();
		}
	}
}

bool UHoundAttackNotify::CheckPlayerIsHit( AASMeleeEnemy* pcHound )
{
	//Get the distance between the hound and the player
	APawn* pcPlayerPawn = pcHound->GetWorld()->GetFirstPlayerController()->GetPawn();
	if ( IsValid( pcPlayerPawn ) )
	{
		FVector sPlayerLocation = pcPlayerPawn->GetActorLocation();
		FVector sHoundLocation = pcHound->GetActorLocation();
		FVector sPositionVector = sPlayerLocation - sHoundLocation;

		//check if it's LTE the attack range
		if (sPositionVector.Size() <= pcHound->m_fBasicAttackRadius)
		{
			//Get the position vector (player - hound) and compute the dot product between the (normalized) pos vector and the hound's forward vector.
			sPositionVector.Normalize();
			float fDotProduct = FVector::DotProduct( sPositionVector, pcHound->GetActorForwardVector() );

			//If the dot product is GTE some cos(theta) value treat as a hit	
			if (fDotProduct >= 0.6f)
			{
				UE_LOG( LogTemp, Warning, TEXT( "Hound Hit Player!" ) );
				return true;
			}
			else 
			{
				UE_LOG( LogTemp, Warning, TEXT( "Hound Missed Player!" ) );
			}
		}
	}

	return false;
}
