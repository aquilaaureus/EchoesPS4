// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Includes
#include "CoreMinimal.h"
#include "Actors/EnvironmentObjects/EnvironmentObject.h"
#include "BreakableObject.generated.h"

// Fwd dcl.
class USoundCue;

//Delegate for when the actor is broken (useful for VFX spawns)
DECLARE_DYNAMIC_MULTICAST_DELEGATE( FObjectBroken );

//////////////////////////////////////////////////////////////////////////
// Class ABreakableObject: Base class for all breakable objects within the game.
// Defines any behaviour that all of the breakable objects will have, such as
// taking damage from the player and playing audio que's when they are destroyed.
// To be used correctly, it will be derived into subclasses with specific breakable
// behaviour, however this can be used to directly to create breakable objects that
// will just disappear without any extra behaviour.
//////////////////////////////////////////////////////////////////////////
UCLASS()
class ECHOES_API ABreakableObject : public AEnvironmentObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABreakableObject();

	// A function UE4 has to control the damage the object will take from the character
	virtual	float TakeDamage( float fDamageAmount, FDamageEvent const& krsDamageEvent,
		AController* pcEventInstigator, AActor* pcDamageCauser ) override;

	virtual void BeginPlay() override;

	UFUNCTION()
	void ReplaceWithBroken(AActor * pcActor, EEndPlayReason::Type EReason);

	//Delegate that broadcasts when the object is destroyed
	UPROPERTY( BlueprintAssignable, Category = "Event" )
	FObjectBroken m_dObjectBroken;

protected:

	// A variable to add the sound file
	UPROPERTY( EditAnywhere, Category = "Designer Values", DisplayName = "Breaking Sound" )
	TArray<USoundCue*> m_apcBreakingSounds;

	//Array of objects, to randomly select one to spawn
	UPROPERTY(EditDefaultsOnly, Category = "Broken up Actors for replacement on Destruction:")
	TArray<TSubclassOf<AActor>> m_pcBrokenActorsArray;

};
