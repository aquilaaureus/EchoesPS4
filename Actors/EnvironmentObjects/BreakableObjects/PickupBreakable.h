// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Includes
#include "CoreMinimal.h"
#include "Actors/EnvironmentObjects/BreakableObjects/BreakableObject.h"
#include "PickupBreakable.generated.h"

// Fwd dcl.
class APickup;
struct FDamageEvent;

//////////////////////////////////////////////////////////////////////////
// Class APot: One specific type of breakable object within the game that will
// spawn pickups for the player when it is broken. These are set through an exposed
// array from which one of the pickups will be spawned. To be used correctly, A
// Blueprint subclass will be created that will set default values and also set
// the static mesh that this pot will use.
//////////////////////////////////////////////////////////////////////////
UCLASS()
class ECHOES_API APickupBreakable : public ABreakableObject
{
	GENERATED_BODY()

private:
	// A function to control the spawning of the pickups after the pot is broken
	void SpawnItems( void );

protected:
	// The actor class that forms the mana pickup to be spawned when the pot is broken
	UPROPERTY( EditAnywhere, Category = "Designer Values", DisplayName = "Spawn Items" )
	TArray<TSubclassOf<APickup>> m_pacSpawnableItems;

	// The spawn radius around the pot in which pickups will spawn
	UPROPERTY( EditAnywhere, Category = "Designer Values", DisplayName = "Spawn Radius" )
	float m_fSpawnRadius;

public:	
	// Sets default values
	APickupBreakable();

	// A function UE4 has to control the damage the Pot will take from the character
	virtual	float TakeDamage( float fDamageAmount, FDamageEvent const& krsDamageEvent, 
		AController* pcEventInstigator, AActor* pcDamageCauser ) override;

};
