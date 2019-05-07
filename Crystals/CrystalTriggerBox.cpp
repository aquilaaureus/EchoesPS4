// Fill out your copyright notice in the Description page of Project Settings.

#include "CrystalTriggerBox.h"
#include "Actors/Kira/EchoesCharacter.h"
#include <Kismet/GameplayStatics.h>


//////////////////////////////////////////////////////////////////////////
// Constructor used to set tick parameters
//////////////////////////////////////////////////////////////////////////
ACrystalTriggerBox::ACrystalTriggerBox ( const FObjectInitializer& rcObjectInitializer )
	: Super ( rcObjectInitializer )
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}


//////////////////////////////////////////////////////////////////////////
// Called when the game starts to be played. Used for setup
//////////////////////////////////////////////////////////////////////////
void ACrystalTriggerBox::BeginPlay ( )
{
	Super::BeginPlay ( );

	// Set the actor tick enabled
	SetActorTickEnabled ( true );
}




//////////////////////////////////////////////////////////////////////////
// Called once per frame of the game
//////////////////////////////////////////////////////////////////////////
void ACrystalTriggerBox::Tick ( float fDeltaTime )
{
	Super::Tick ( fDeltaTime );
}

//////////////////////////////////////////////////////////////////////////
// A callback function called when another actor overlaps this one
//////////////////////////////////////////////////////////////////////////
void ACrystalTriggerBox::NotifyActorBeginOverlap ( AActor* pcOtherActor )
{
	//check if the overlapped actor is player only
	if ( Cast<AEchoesCharacter> ( pcOtherActor ) )
	{
		SpawnCrystals ( );
	}
}


void ACrystalTriggerBox::SpawnCrystals ( )
{
	//if crystal object is not setup, return
	if ( !m_pcIceCrystal ) { return; }

	ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter ( GetWorld ( ) , 0 );

	FTransform playerTransform = playerCharacter->GetTransform ( );

	for ( int i = 0; i<3; i++ )
	{
		FVector forwardVector;
		FTransform spawnTransform = playerCharacter->GetTransform ( );
		FVector spawnLocation = spawnTransform.GetLocation ( );

		switch ( i )
		{
			case 0:
				forwardVector = playerCharacter->GetActorForwardVector ( );
				spawnLocation -= ( forwardVector * m_ftDistanceFactor );
				break;
			case 1:
				forwardVector = playerCharacter->GetActorRightVector ( );
				spawnLocation -= ( forwardVector * m_ftDistanceFactor );
				break;
			default:
				forwardVector = playerCharacter->GetActorRightVector ( );
				spawnLocation += ( forwardVector * m_ftDistanceFactor );
				break;
		}

		spawnLocation.Z += ( 2*m_ftDistanceFactor );
		spawnTransform.SetLocation ( spawnLocation );

		UE_LOG ( LogTemp , Warning , TEXT ( "Player transform : %f , %f , %f " ) , forwardVector.X , forwardVector.Y , forwardVector.Z )
			UE_LOG ( LogTemp , Warning , TEXT ( "transform : %f , %f , %f " ) , spawnTransform.GetLocation ( ).X , spawnTransform.GetLocation ( ).Y , spawnTransform.GetLocation ( ).Z )

			GetWorld ( )->SpawnActor<AActor> ( m_pcIceCrystal , spawnTransform );
	}

}
