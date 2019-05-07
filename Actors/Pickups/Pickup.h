// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Includes
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

// Fwd dcl.
class AEchoesCharacter;
class UPlayerStatsComponent;
class USphereComponent;
class UStaticMeshComponent;

//////////////////////////////////////////////////////////////////////////
// Class APickup: The base class for all pickup objects within the game, defines
// functionality and behaviour common to all the pickups such as the magnetism with
// the player. It also provides a pure virtual function to be overridden by sub 
// classes which will contain the pickup specific behaviour such as replenishing
// health.
//////////////////////////////////////////////////////////////////////////
UCLASS( abstract )
class ECHOES_API APickup : public AActor
{
	GENERATED_BODY()
	
private:
	// Controls whether this pickup is currently active to be picked up by the player
	bool m_bIsActive;

protected:
	// A variable to store the collision box of the pickup
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "StaticMeshComponent" )
	UStaticMeshComponent* m_pcStaticMeshComp;

	// The area in which magnetism will be aplied to the pickup if the player is inside it
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "SphereCollision" )
	USphereComponent* m_pcSphereComponent;

	// A pointer to the player character so that the it can be affected once the pickup is collected
	UPROPERTY()
	AEchoesCharacter* m_pcPlayer;

	// A pointer to the component handling the player stats, which pickups affect
	UPROPERTY()
	UPlayerStatsComponent* m_pcPlayerStatsComp;

	// A variable to store the value of how fast the pickup will go to the player.
	UPROPERTY( EditDefaultsOnly, Category = "Designer Values", DisplayName = "Speed of Magnetism" )
	float m_fSpeed;

	// How close the player the pickup should be before its collected
	UPROPERTY( EditDefaultsOnly, Category = "Designer Values", DisplayName = "Collection Distance" )
	float m_fDisappearDistance;

	// The radius of the sphere used for the pickup magnestism
	UPROPERTY( EditDefaultsOnly, Category = "Designer Values", DisplayName = "Magnetism Radius" )
	float m_fMagnetismRadius;

	// The amount of time (in seconds) before the pickup is active
	UPROPERTY( EditDefaultsOnly, Category = "Designer Values", DisplayName = "Active Delay" )
	float m_fDelay;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Toggles the object on and off based on the parameter
	void ToggleActive( bool bActive );

	// Check to see if the object should be activated or not
	UFUNCTION()
	void CBCheckActivation();

	// A function to handle the specific behaviour of pickups when collected
	virtual void PickupCollected() PURE_VIRTUAL( APickup::PickupCollected, ; );

	// A function to decide if the pickup should float toward the player or not
	// Return: True if the pickup should float toward the player, false otherwise
	virtual bool ShouldBeCollected() PURE_VIRTUAL( APickup::ShouldBeCollected, return false; );

public:
	// Sets default values for this actor's properties
	APickup();

	// Called every frame
	virtual void Tick( float fDeltaTime ) override;

	// Delegate function to be called when player overlaps the collider
	UFUNCTION()
	void CBOnOverlapBegin( UPrimitiveComponent* pcHitComp, AActor* pcOtherActor, UPrimitiveComponent* pcOtherComp,
		int32 iOtherBodyIndex, bool bFromSweep, const FHitResult& rsSweepResult );
};
