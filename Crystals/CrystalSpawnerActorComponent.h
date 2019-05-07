// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CrystalSpawnerActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECHOES_API UCrystalSpawnerActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCrystalSpawnerActorComponent();

	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float fDeltaTime, ELevelTick eTickType, FActorComponentTickFunction* sThisTickFunction) override;

	// The crystal that should be spawned
	UPROPERTY ( EditAnywhere , Category = "Crystal Spawner" , DisplayName = "Spawned Actor" )
	TSubclassOf<AActor> m_pcIceCrystal;

	//distance on X and Y axis at which the actor will spawn
	UPROPERTY ( EditAnywhere , Category = "Crystal Spawner" , DisplayName = "Distance Factor" )
	float m_fDistanceFactor;

	//boolean to check weather to spawn the crystals from bottom
	UPROPERTY ( EditAnywhere , Category = "Crystal Spawner" , DisplayName = "Spawn From Bottom" )
	bool m_bIsFromBottom;

	//boolean to check weather should wait for the previous spawned crystals to destroy first
	UPROPERTY ( EditAnywhere , Category = "Crystal Spawner" , DisplayName = "Wait for Previous to Destroy" )
	bool m_bShouldWaitForDestroy;

	// Spawns crystals at particular locations around the player
	void SpawnCrystals ( );

	// Spawns crystal at a given transform and a duration to de-spawn after if given.
	// N.B. -1.0 timer indicates do not despawn
	void SpawnCrystalWithTransform(FTransform sSpawnTransform, float fDespawnTimer = (-1.0f));

private:

	void CheckCrystalStatus ( );

	// Checks and updates actors in m_asTimedDespawningActors, removing actors when dead
	void UpdateDespawningActors(float fDeltaTime);

	// An array containing all of the crystals that have been spawned
	TArray<AActor*> m_apcSpawnedCrystals;

	// Array of pairs for despawning
	UPROPERTY()
	TArray<AActor*> m_apcTimedDespawningActors;

	// Array of floats to match despawning actors (pair structs could not be used with reflection system)
	TArray<float> m_afDespawningActorsTimers;
	
};
