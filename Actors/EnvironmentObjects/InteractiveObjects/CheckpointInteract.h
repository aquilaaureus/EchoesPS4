// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Includes
#include "CoreMinimal.h"
#include "Actors/EnvironmentObjects/InteractiveObjects/InteractableObject.h"
#include "CheckpointInteract.generated.h"

// Fwd. dcl
class UParticleSystem;
class UParticleSystemComponent;
class UStaticMeshComponent;
class USoundCue;
class UUserWidget;

//////////////////////////////////////////////////////////////////////////
// Class AInteractableFound: A specific type of interactable object within the 
// game. The purpose of this is to function as the checkpoint and health restore
// system within the game. When the player first interacts with it the crystals 
// on the top will be destroyed and the respawn location set, every subsequent 
// interaction will just set the respawn location of the player and restore some
// of their health. To use a blueprint subclass will needed to set both mesh
// components.
//////////////////////////////////////////////////////////////////////////
UCLASS()
class ECHOES_API ACheckpointInteract : public AInteractableObject
{
	GENERATED_BODY()
	
private:
	// The instance of the Loading Widget class.
	UPROPERTY()
	UUserWidget* m_pcLoadingWidgetInstance;

	// A boolean to control if the fountain has been recently activated
	bool m_bRecentlyActivated;

	// Create the widget from class uproperty if valid, else log errors.
	void InstantiateLoadWidget();

	// Start load widget animation (self-removes from parent) - only meant to fire once per object
	void PlayLoadWidget();

protected:
	// The component that will be used as the breakable portion of the object
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "BreakableMesh" )
	UStaticMeshComponent* m_pcBreakableMesh;

	// The particle system component that will play the particle effects
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "ParticleSystemComponent" )
	UParticleSystemComponent* m_pcParticleSystemComponent;

	// The widget that appears to indicate the game is currently setting a checkpoint
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Loading Widget Class" )
	TSubclassOf<UUserWidget> m_pcLoadingWidgetClass;

	// The particle system used when the fountain is interacted with/activated
	UPROPERTY( EditAnywhere, Category = "Designer Values", DisplayName = "Activated PS" )
	UParticleSystem* m_pcActivatedPS;

	// The particle system used when the fountain has been activated
	UPROPERTY( EditAnywhere, Category = "Designer Values", DisplayName = "Active PS" )
	UParticleSystem* m_pcActivePS;

	// The particle system used before the fountain has been activated
	UPROPERTY( EditAnywhere, Category = "Designer Values", DisplayName = "Passive PS" )
	UParticleSystem* m_pcPassivePS;
	
	// The Idle SFX for the Checkpoint
	UPROPERTY( EditAnywhere, Category = "Designer Values", DisplayName = "Idle SFX" )
	USoundCue* m_pcIdleSfx;

	// The Active SFX for the Checkpoint
	UPROPERTY( EditAnywhere, Category = "Designer Values", DisplayName = "Active SFX" )
	USoundCue* m_pcActiveSfx;

public:
	// Sets default values for this actor's properties
	ACheckpointInteract();

	// Callback function for when the player interacts with the object
	virtual void CBInteract() override;

	// Beingplay override for icon spawn
	virtual void BeginPlay() override;

	// Called once per frame
	virtual void Tick( float fDeltaTime ) override;
};
