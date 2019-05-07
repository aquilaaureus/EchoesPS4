// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Includes
#include "CoreMinimal.h"
#include "Actors/EnvironmentObjects/EnvironmentObject.h"
#include "EmergingHazard.generated.h"

// Fwd dcl
class UBoxComponent;
class UDecalComponent;

//////////////////////////////////////////////////////////////////////////
// Class AEmergingHazard: An abstract class used for any object that is going to be
// emerging from the floor. This controls behaviour that is common to all such as 
// triggering and then coming out of the floor. It also calls a pure virtual function
// once it has emerged that will be defined in specific subclasses with any behaviour
// that is specific to that object.
//////////////////////////////////////////////////////////////////////////
UCLASS( abstract )
class ECHOES_API AEmergingHazard : public AEnvironmentObject
{
	GENERATED_BODY()
	
private:
	// The location that the actor starts at
	FVector m_sStartLocation;

	// The start location with the height of the mesh added on to it
	FVector m_sTargetLocation;

	// The height of the collision box that the player will be walking into
	float m_fCollisionBoxHeight;

	// The multiplier used to control how fast crystals emerge
	float m_fInterpolationMultiplier;

	// Boolean value controls whether the crystal is currently emerging
	bool m_bIsEmerging;

	// The Minimum Boundary for the Random Multiplier of the Decal's size
	UPROPERTY( EditAnywhere, Category = "Designer Values", DisplayName = "Decal Size Min Random Multiplier" )
	float m_fDecalRandomMultiplierMinBound;

	// The Maximum Boundary for the Random Multiplier of the Decal's size
	UPROPERTY( EditAnywhere, Category = "Designer Values", DisplayName = "Decal Size Max Random Multiplier" )
	float m_fDecalRandomMultiplierMaxBound;

protected:

	// The World Space Location of the Decal that must always be the position of the Decal
	FVector m_sDecalWorldLocation;

	// The collision box used to activate the hazard
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "CollisionBox" )
	UBoxComponent* m_pcCollisionBox;

	// A component used to project decals onto the object
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "DecalComponent" )
	UDecalComponent* m_pcDecalComponent;

	// The delay before the crystal emerges from the floor after it's triggered
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Designer Values", DisplayName = "Delay Time" )
	float m_fDelayTime;

	// How long it takes for the crystal to emerge
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Designer Values", DisplayName = "Emerge Time" )
	float m_fEmergeTime;

	// The current amount of interpolation that has been applied
	float m_fCurrentInterpolation;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Function to be overwritten in children to control behaviour for after the hazard has emerged
	virtual void EmergeCompleted() PURE_VIRTUAL( AEmergingHazard::EmergeCompleted, ; );

	// Function used to by child classes to access the Start Location
	const FVector& GetEmergeStartLocation() const { return m_sStartLocation; };

	// Function used to by child classes to access the Target Location
	const FVector& GetEmergeTargetLocation() const { return m_sTargetLocation; };

public:
	// Set default values
	AEmergingHazard();

	// Called every frame
	virtual void Tick( float fDeltaTime ) override;

	// Delegate function to be called when player overlaps the collider
	UFUNCTION()
	virtual void CBOnOverlapBegin( UPrimitiveComponent* pcHitComp, AActor* pcOtherActor, UPrimitiveComponent* pcOtherComp,
		int32 iOtherBodyIndex, bool bFromSweep, const FHitResult& rsSweepResult );

	// Callback function to trigger the crystal exploding from the ground
	UFUNCTION()
	void CBEmergeDelayEnded();

};
