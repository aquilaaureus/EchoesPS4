// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Includes
#include "CoreMinimal.h"
#include "Actors/EnvironmentObjects/EmergingObjects/EmergingHazard.h"
#include "EmergingDamageHazard.generated.h"

// Fwd dcl.
class UParticleSystemComponent;
class UParticleSystem;
class URadialForceComponent;
class USoundAttenuation;
class USoundCue;

//////////////////////////////////////////////////////////////////////////
// Class AEmergingDamageHazard: This is a hazard within the game that springs from the
// ground when the player passes over it. It fires after a short delay when triggered by 
// the player. If the player stays on the crystal they will take damage. After a small amount
// of time the crystal will sink back beneath the surface to prepare to strike again. In order 
// to use class, a blueprint object needs to be made from it, so that the static mesh can
// be set. 
//////////////////////////////////////////////////////////////////////////
UCLASS()
class ECHOES_API AEmergingDamageHazard : public AEmergingHazard
{
	GENERATED_BODY()
	
private:	
	// The actor that triggered the spikes to be pop up
	UPROPERTY( Transient )
	AActor* m_pcOverlappingActor;

	// The multiplier used to control how fast crystals sink
	float m_fSinkingMultiplier;

	// Boolean value controls whether the crystal is currently sinking
	bool m_bIsSinking;

protected:

	// The upper knockback force component
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "ForceKnockback" )
	URadialForceComponent* m_pcForceKnockbackUpper;

	// The upper knockback force component
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "ForceKnockback" )
	URadialForceComponent* m_pcForceKnockbackLower;

	// How long it takes for the crystal to emerge
	UPROPERTY( EditDefaultsOnly, Category = "Designer Values|Force Knockback", DisplayName = "Knockback Force" )
	float m_fKnockbackForce;

	// The radius of the force knockback components
	UPROPERTY( EditDefaultsOnly, Category = "Designer Values|Force Knockback", DisplayName = "Knockback Radius" )
	float m_fKnockbackRadius;

	UPROPERTY( EditDefaultsOnly, Category = Particles )
	UParticleSystem* m_pcDerbisPartSys;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "ParticleSystem" )
	UParticleSystemComponent* m_pcSpawnedParticleSystem;

	// The sound the spikes make when they emerge
	UPROPERTY( EditDefaultsOnly, Category = "Designer Values", DisplayName = "Emerge Sound" )
	USoundCue* m_pcEmergeSound;

	// The sound the spikes make when they retract
	UPROPERTY( EditDefaultsOnly, Category = "Designer Values", DisplayName = "Retract Sound" )
	USoundCue* m_pcRetractSound;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Function to be overwritten in children to control behaviour for after the hazard has emerged
	virtual void EmergeCompleted() override;

public:	
	// How long it takes for the crystal to sink
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Designer Values", DisplayName = "Sink Time" )
	float m_fSinkTime;

	// The amount of time the crystal remains out of ground before it sinks
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Designer Values", DisplayName = "Active Time" )
	float m_fActiveTime;

	// The amount of damage the crystals deal if they collide with the player
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Designer Values", DisplayName = "Damage Dealt" )
	float m_fDamageDealt;

	// Sets default values for this actor's properties
	AEmergingDamageHazard();

	// Called every frame
	virtual void Tick( float fDeltaTime ) override;

	// Delegate function to be called when player overlaps the collider
	UFUNCTION()
	virtual void CBOnOverlapBegin( UPrimitiveComponent* pcHitComp, AActor* pcOtherActor, UPrimitiveComponent* pcOtherComp,
		int32 iOtherBodyIndex, bool bFromSweep, const FHitResult& rsSweepResult ) override;

	// Callback function to trigger the crystal sinking beneath the ground
	UFUNCTION()
	void CBActiveTimeEnded();
};
