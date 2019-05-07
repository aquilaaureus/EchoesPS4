// Fill out your copyright notice in the Description page of Project Settings.

// Header Include
#include "PickupBreakable.h"

// Other Includes
#include "Engine/World.h"
#include "Actors/Pickups/Pickup.h"

//////////////////////////////////////////////////////////////////////////
// Sets default values
//////////////////////////////////////////////////////////////////////////
APickupBreakable::APickupBreakable()
: m_fSpawnRadius	( 200.f )
{ }

//////////////////////////////////////////////////////////////////////////
// A function UE4 has to control the damage the Pot will take from the character
//////////////////////////////////////////////////////////////////////////
float APickupBreakable::TakeDamage( float fDamageAmount, FDamageEvent const& krsDamageEvent, AController* pcEventInstigator, AActor* pcDamageCauser )
{
	// Spawns the items for the player to pick up
	SpawnItems();

	// Call the base class function for base breakable functionality
	ABreakableObject::TakeDamage( fDamageAmount, krsDamageEvent, pcEventInstigator, pcDamageCauser );

	return fDamageAmount;
}

//////////////////////////////////////////////////////////////////////////
// A function to control the spawning of the pickups after the pot is broken
//////////////////////////////////////////////////////////////////////////
void APickupBreakable::SpawnItems()
{
	// Randomly spawn an item from the array, if there are items in the array
	if( m_pacSpawnableItems.Num() > 0 )
	{
		// Randomly choose a pickup from the array, and spawn randomly within a radius around the pot
		int iIndex = FMath::FRandRange( 0, m_pacSpawnableItems.Num() );

		FVector sSpawnPosition;
		sSpawnPosition = FVector( FMath::FRandRange( -m_fSpawnRadius, m_fSpawnRadius ), FMath::FRandRange( -m_fSpawnRadius, m_fSpawnRadius ), 200.f );

		GetWorld()->SpawnActor< AActor >( m_pacSpawnableItems[ iIndex ], GetActorLocation() + sSpawnPosition, GetActorRotation() );
	}
}

