// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Includes
#include "CoreMinimal.h"
#include "Actors/EnvironmentObjects/EmergingObjects/EmergingHazard.h"
#include "EmergingKnockbackHazard.generated.h"

// Fwd dcl.
class URadialForceComponent;

//////////////////////////////////////////////////////////////////////////
// Class AEmergingKnockbackCrystal: This is a knockback hazard within the game that
// explodes from the ground when the player walks over its trigger box. When it appears
// a crystal static mesh becomes permanently formed that the player cannot pass, and it
// fires a force knockback out to knock the player back. It also uses a decal to provide
// a visual shadow on the ground where the hazard is so that the player is aware of the
// danger before they walk over it. In order to use this class it will need to be
// subclassed as a blueprint so that the static mesh that it uses can be set. 
//////////////////////////////////////////////////////////////////////////
UCLASS()
class ECHOES_API AEmergingKnockbackHazard : public AEmergingHazard
{
	GENERATED_BODY()
	
protected:
	// The upper knockback force component
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "ForceKnockback" )
	URadialForceComponent* m_pcForceKnockbackUpper;

	// The upper knockback force component
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "ForceKnockback" )
	URadialForceComponent* m_pcForceKnockbackLower;

	// How long it takes for the crystal to emerge
	UPROPERTY( EditDefaultsOnly, Category = "Designer Values", DisplayName = "Knockback Force" )
	float m_fKnockbackForce;

	// The radius of the force knockback components
	UPROPERTY( EditDefaultsOnly, Category = "Designer Values", DisplayName = "Knockback Radius" )
	float m_fKnockbackRadius;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Function to be overwritten in children to control behaviour for after the hazard has emerged
	virtual void EmergeCompleted() override;

public:	
	// Sets default values for this actor's properties
	AEmergingKnockbackHazard();
};
