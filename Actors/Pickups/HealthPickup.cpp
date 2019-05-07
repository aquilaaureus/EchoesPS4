// Fill out your copyright notice in the Description page of Project Settings.

// Header include
#include "HealthPickup.h"

// Other includes
#include "Actors/Kira/EchoesCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerComponents/PlayerStatsComponent.h"

//////////////////////////////////////////////////////////////////////////
// Sets default values
//////////////////////////////////////////////////////////////////////////
AHealthPickup::AHealthPickup()
: m_fHealthPoints( 20 ) // The amount of health the health pickup add to the player
{}


//////////////////////////////////////////////////////////////////////////
// A function to handle the specific behavior of pickups when collected
//////////////////////////////////////////////////////////////////////////
void AHealthPickup::PickupCollected()
{
	if (m_pcPlayerStatsComp)
	{
		m_pcPlayerStatsComp->AddHealth(m_fHealthPoints);
		Destroy();
	}
}

//////////////////////////////////////////////////////////////////////////	
// A function to decide if the pickup should float toward the player or not
//////////////////////////////////////////////////////////////////////////
bool AHealthPickup::ShouldBeCollected()
{
	if( m_pcPlayerStatsComp )
	{
		if( m_pcPlayerStatsComp->GetPlayerCurrentHealth() < m_pcPlayerStatsComp->GetMaxHealth() )
		{
			return true;
		}
	}

	return false;
}