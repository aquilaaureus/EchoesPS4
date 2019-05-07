// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Includes
#include "CoreMinimal.h"
#include "Actors/EnvironmentObjects/EnvironmentObject.h"
#include "InteractableObject.generated.h"

// Fwd Dcl.
class AEchoesCharacter;
class AUIEventDispatcher;
class UBoxComponent;
class USphereComponent;
class UTexture2D;
class UBillboardComponent;
class UStaticMeshComponent;
class UUserWidget;

//////////////////////////////////////////////////////////////////////////
// Class AInteractableObject: This class forms the base for the interactable objects
// within the game. It uses a sphere component that will track when the player has
// entered the area around the object. It a Billboard component to display an interaction
// icon and a callback that overrides base input behaviour for the player within the
// sphere. In order to be appropriately used, the CBInteract function should be 
// overriden with specific behaviour in a subclass of this object, but the base class
// function must also be called.
//////////////////////////////////////////////////////////////////////////
UCLASS()
class ECHOES_API AInteractableObject : public AEnvironmentObject
{
	GENERATED_BODY()

private:
	// Sets up the pointer to the map event dispatcher used to fire events
	void SetupEventDispatcherMember();

protected:
	// A pointer to the character that enters the interact zone
	AEchoesCharacter* m_pcPlayerCharacter;

	// A pointer to the actor used to dispatch map events
	UPROPERTY( Transient )
	AUIEventDispatcher* m_pcUIEventDispatcher;

	// The value of the Inverse Cos of the Angle between the player and the chest
	UPROPERTY()
	float m_fInvCos;

	// Can the interactable be interacted with more than once?
	UPROPERTY( EditAnywhere, Category = "Designer Values", DisplayName = "Multiple Activation" )
	bool m_bMultipleActivation;

	// The ID used for the map annotation detail
	UPROPERTY( EditAnywhere, Category = "Designer Values", DisplayName = "Map Annotation ID" )
	FString m_strAnnotationID;

	// Determines if there is a Box Collider attached
	UPROPERTY()
	bool m_bHasBoxCollider;

	// If the player is overlapping a colldier, allow child classes to know this 
	UPROPERTY()
	bool m_bIsPlayerOverlapping;

	// Determines if the player is facing the Interactable
	UPROPERTY()
	bool m_bIsPlayerFacing;

	// Has the Input been enabled?
	UPROPERTY()
	bool m_bInputEnabled;

	// Do any Dynamics need to be removed
	UPROPERTY()
	bool m_bNeedToRemoveDynamics;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Call to fire the event to update the map
	void FireMapEvent();

	// Function to allow the player to Interact ONLY if they are facing the interactable
	void EnableInteract();

	// Remove and call ResetInput only if there are Sphere Colliders
	void RemoveSphereDynamics();

	// Enable/Disable Input to the Pawn of the Player
	void SetPlayerInput( bool bInputState );

	// Resets the value of fInvCos;
	void ResetInvCos() { m_fInvCos = 0.f; };

	// Resets the input after the player has interacted or left the area
	void ResetInput();

	// Function to remove the standard Sphere Component and add dynamic callbacks to a Box Component
	UBoxComponent* SetBoxDynamics( UBoxComponent* pcBoxCollider );

	// Function to remove dynamic callbacks to a Box Component	
	UBoxComponent* RemoveBoxDynamics( UBoxComponent* pcBoxCollider );

public:	
	// A collision box component for detecting when the player steps on the platform
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "InteractionSphere" )
	USphereComponent* m_pcInteractionSphere;

	// The billboard component used to display an interact icon
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "InteractIcon" )
	UBillboardComponent* m_pcBillboardComponent;

	// The amount to be added to the radius to create a large enough zone around the static mesh
	UPROPERTY( EditAnywhere, Category = "Designer Values", DisplayName = "Radius Increase" )
	float m_fRadiusIncrease;

	// The amount of height added to the billboard to position above the object
	UPROPERTY( EditAnywhere, Category = "Designer Values", DisplayName = "Billboard Height" )
	float m_fZAddition;

	// Sets default values for this actor's properties
	AInteractableObject();

	// Delegate function to be called when player overlaps the collider
	UFUNCTION()
	void CBOnOverlapBegin( UPrimitiveComponent* pcHitComp, AActor* pcOtherActor, UPrimitiveComponent* pcOtherComp,
		int32 iOtherBodyIndex, bool bFromSweep, const FHitResult& rsSweepResult );

	// Delegate function to be called when the player leaves the collider
	UFUNCTION()
	void CBOnOverlapEnd( UPrimitiveComponent* pcOverlappedComp, AActor* pcOtherActor, 
		UPrimitiveComponent* pcOtherComp, int32 iOtherBodyIndex );

	// Callback function for when the player interacts with the object
	UFUNCTION()
	virtual void CBInteract();
};
