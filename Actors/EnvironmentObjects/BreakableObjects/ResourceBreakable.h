// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Includes
#include "CoreMinimal.h"
#include "Actors/EnvironmentObjects/BreakableObjects/BreakableObject.h"
#include "ResourceBreakable.generated.h"

// Fwd dcl.
class AResourcePickup;
class UParticleSystem;
class USoundCue;
class UStaticMesh;

//////////////////////////////////////////////////////////////////////////
// Class ABreakableIce: A specific type of breakable within the game that will reward
// the player with a random amount of dust between two set values when destroyed by
// the player. To use correctly, make a blueprint subclass to set class members such
// as the static mesh, and then set dust drop values on individual instances of the
// breakable.
//////////////////////////////////////////////////////////////////////////
UCLASS()
class ECHOES_API AResourceBreakable : public ABreakableObject
{
	GENERATED_BODY()

protected:
	// The class from which to spawn the pickup from
	UPROPERTY( EditDefaultsOnly, Category = "Designer Values", DisplayName = "Resource Pickup" )
	TSubclassOf<AResourcePickup> m_pcResourcePickup;

	UPROPERTY(EditDefaultsOnly, Category = "Designer Values", DisplayName = "Health")
	int m_iHealth;

	// Controls whether this crystal will drop a shard or not
	UPROPERTY( EditAnywhere, Category = "Designer Values", DisplayName = "Drops Resource" )
	bool m_bDropsShard;

	//Damage VFX
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	UParticleSystem* m_pcDamagePatricleEmitter;

	// A variable to add the sound file
	UPROPERTY(EditAnywhere, Category = "Designer Values", DisplayName = "Cracked Sound")
	USoundCue* m_pcDamageSound;

	// Called when play starts or actor spawned
	virtual void BeginPlay() override;

	//Array of Broken Texture Materials for various levels (integers) of health
	UPROPERTY(EditAnywhere, Category = "Damaged", DisplayName = "Meshes Array")
	TArray<UStaticMesh*> m_apcHealthMeshes;

	//Max deflection angle when shaking
	UPROPERTY(EditAnywhere, Category = "Damaged", DisplayName = "Angle")
	float m_fMaxDeflectionAngle;

	//Duration of Shaking effect
	UPROPERTY(EditAnywhere, Category = "Damaged", DisplayName = "Duration (ms)")
	float m_fShakeMs;

	bool m_bShaking;

	float m_fShakeTime;

	FVector m_v3fRotation;

	FTransform m_sTransform;

	//Crystal Stubs Actor
	UPROPERTY(EditDefaultsOnly, Category = "Broken up Actors for replacement on Destruction", DisplayName = "Crystal Stubs")
	TSubclassOf<AActor> m_pcStubsActor;

	//Crystal Stubs Scale
	UPROPERTY(EditDefaultsOnly, Category = "Broken up Actors for replacement on Destruction", DisplayName = "Stubs Scale")
	float m_fStubsScale;

public:
	// Sets default values
	AResourceBreakable();

	// A function UE4 has to control the damage the ice will take from the character
	virtual	float TakeDamage( float fDamageAmount, FDamageEvent const& krsDamageEvent,
		AController* pcEventInstigator, AActor* pcDamageCauser ) override;
	
	virtual void Tick(float fTimeDelta) override;
};
