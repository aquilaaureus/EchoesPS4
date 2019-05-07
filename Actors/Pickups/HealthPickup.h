// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Includes
#include "CoreMinimal.h"
#include "Actors/Pickups/Pickup.h"
#include "HealthPickup.generated.h"

//////////////////////////////////////////////////////////////////////////
// Class AHealthPickup: A specific type of pickup that will restore health to
// the player. It will only restore health of the player is not already at max
// health, otherwise it will remain on the ground to be pickup later.
//////////////////////////////////////////////////////////////////////////
UCLASS()
class ECHOES_API AHealthPickup : public APickup
{
	GENERATED_BODY()
	
protected:
	// The amount of health to add when this pickup is collected
	UPROPERTY( EditDefaultsOnly, Category = "Designer Values", DisplayName = "Health Points" )
	float m_fHealthPoints;

	// A function to handle the specific behaviour of pickups when collected
	virtual void PickupCollected() override;

	// A function to decide if the pickup should float toward the player or not
	virtual bool ShouldBeCollected() override;

public:
	// Sets default values
	AHealthPickup();
};
