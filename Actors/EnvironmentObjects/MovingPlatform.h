// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Includes
#include "CoreMinimal.h"
#include "Actors/EnvironmentObjects/EnvironmentObject.h"
#include "MovingPlatform.generated.h"

// Fwd dcl
class USphereComponent;

//////////////////////////////////////////////////////////////////////////
// Enum Class EMovingStatus: Stores the actions that the moving platform can undertake
//////////////////////////////////////////////////////////////////////////
enum class EMovingStatus
{
	EMovingStatus_Forwards,
	EMovingStatus_Backwards,
	EMovingStatus_Halted
};

//////////////////////////////////////////////////////////////////////////
// Class AMovingPlatform: A platform that moves between two points to carry the player
// between areas. It uses interpolation to control its movement based on some editor 
// values that can be set to control how fast it moves and how long it stays at each
// end before moving again. It also uses a collision component that doesn't register
// events to provide the target location in editor.
//////////////////////////////////////////////////////////////////////////
UCLASS()
class ECHOES_API AMovingPlatform : public AEnvironmentObject
{
	GENERATED_BODY()

private:
	// The start location of the moving platform
	FVector m_sStartLocation;

	// The target location that the platform is moving to
	FVector m_sTargetLocation;

	// The current status of this moving platform
	EMovingStatus m_eCurrentStatus;

	// The next state of this moving platform
	EMovingStatus m_eNextState;

	// The amount of interpolation to apply on this frame
	float m_fCurrentInterpolation;

	// The multiplier used to make movement take the set amount of time
	float m_fMovementMultiplier;

protected:
	// The shape component used to determine the target location
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "TargetLocation" )
	USphereComponent* m_pcTargetLocation;

	// The amount of time the platform halts for when it reaches its destination
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Designer Values", DisplayName = "Halt Time" )
	float m_fHaltTime;

	// How long (in seconds) it takes to move from one location to the other
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Designer Values", DisplayName = "Move Time" )
	float m_fMoveTime;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Sets default values for this actor's properties
	AMovingPlatform();

	// Called every frame
	virtual void Tick( float fDeltaTime ) override;

	// A callback function for when the halt timer finishes
	UFUNCTION()
	void CBOnHaltTimeEnded();
};
