// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Includes
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CorruptionAura.generated.h"

// Fwd dcl
class UBoxComponent;
class AEchoesCharacter;

//////////////////////////////////////////////////////////////////////////
// Class ACorruptionAura: The corruption aura class is an area of effect hazard in the game
// taking the form of a corrupting "cloud". While the player is in the area they will take 
// corruption damage over time based on values said in the editor. To use this class, it only 
// needs to be dragged into the editor and be sized accordingly, it does not need a blueprint
// subclass.
//////////////////////////////////////////////////////////////////////////
UCLASS()
class ECHOES_API ACorruptionAura : public AActor
{
	GENERATED_BODY()

private: 
	// A pointer to the character that has entered the corruption zone
	AEchoesCharacter* m_pcPlayerCharacter;

	// The amount of time that has elapsed since the player last took damage
	float m_fElapsedCorruptionTime;

public:	
	// A collision box component for detecting when the player steps on the platform
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "CollisionBox" )
	UBoxComponent* m_pcCollisionBox;

	// The time it takes in seconds for the player to be damaged by corruption
	UPROPERTY( EditAnywhere, Category = "Designer Values", DisplayName = "Tick Time" )
	float m_fTickTime;

	// The amount of damage dealt to the player per tick of corruption
	UPROPERTY( EditAnywhere, Category = "Designer Values", DisplayName = "Damage To Deal" )
	float m_fDamage;

	// Sets default values for this actor's properties
	ACorruptionAura();

	// Called once per frame as the game loops
	// Param1: The amount of time since the last call of tick
	virtual void Tick( float fDeltaTime ) override;

	// Delegate function to be called when player overlaps the collider
	UFUNCTION()
	void CBOnOverlapBegin( UPrimitiveComponent* pcHitComp, AActor* pcOtherActor, UPrimitiveComponent* pcOtherComp,
		int32 iOtherBodyIndex, bool bFromSweep, const FHitResult& rsSweepResult );

	// Delegate function to be called when the player leaves the collider
	UFUNCTION()
	void CBOnOverlapEnd( UPrimitiveComponent* pcOverlappedComp, AActor* pcOtherActor,
		UPrimitiveComponent* pcOtherComp, int32 iOtherBodyIndex );
};