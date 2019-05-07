// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "CrystalTriggerBox.generated.h"

/**
*
*/
UCLASS()
class ECHOES_API ACrystalTriggerBox : public ATriggerBox
{
	GENERATED_BODY ( )

public:

	// Constructor used to set tick parameters
	ACrystalTriggerBox ( const FObjectInitializer& rcObjectInitializer );

	// A callback function called when another actor overlaps this one
	// Param1: The other actor that has triggered this overlap event
	virtual void NotifyActorBeginOverlap ( AActor* pcOtherActor ) override;

	// Called once per frame of the game
	// Param1: The amount of elapsed time since the last frame
	virtual void Tick ( float fDeltaTime ) override;

	// The crystal that should be spawned
	UPROPERTY ( EditAnywhere , Category = "Basic Setup" , DisplayName = "Spawned Actor" )
	TSubclassOf<AActor> m_pcIceCrystal;

	//distance on X and Y axis at which the actor will spawn
	UPROPERTY ( EditAnywhere , Category = "Basic Setup" , DisplayName = "Distance Factor" )
	float m_ftDistanceFactor;

protected:

	// Called when the game starts to be played. Used for setup
	virtual void BeginPlay ( ) override;

private:

	// Spawns crystals at particular locations behing the player
	void SpawnCrystals (  );

};
