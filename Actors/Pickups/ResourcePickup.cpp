// Fill out your copyright notice in the Description page of Project Settings.

// Header include
#include "ResourcePickup.h"

// Other includes
#include "Actors/Kira/EchoesCharacter.h"
#include "Components/SphereComponent.h"
#include "PlayerComponents/PlayerStatsComponent.h"

//////////////////////////////////////////////////////////////////////////
// A function to handle the specific behavior of pickups when collected
//////////////////////////////////////////////////////////////////////////
void AResourcePickup::PickupCollected()
{
	if (m_pcPlayerStatsComp)
	{
		m_pcPlayerStatsComp->AddResources(1);
		Destroy();
	}
}

//////////////////////////////////////////////////////////////////////////
// A function to decide if the pickup should float toward the player or not
//////////////////////////////////////////////////////////////////////////
bool AResourcePickup::ShouldBeCollected()
{
	if( m_pcPlayerStatsComp )
	{
		if( m_pcPlayerStatsComp->GetResourceAmount() < m_pcPlayerStatsComp->GetUpgradeRequired() )
		{
			return true;
		}
	}
	
	return false;
}