// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Pickups/Pickup.h"
#include "ResourcePickup.generated.h"

//////////////////////////////////////////////////////////////////////////
// Class AResourcePickup: A specific type of pickup within the game that rewards
// the player with resources when this is collected. 
//////////////////////////////////////////////////////////////////////////
UCLASS()
class ECHOES_API AResourcePickup : public APickup
{
	GENERATED_BODY()

protected:
	// A function to handle the specific behaviour of pickups when collected
	virtual void PickupCollected() override;

	// A function to decide if the pickup should float toward the player or not
	virtual bool ShouldBeCollected() override;
};
