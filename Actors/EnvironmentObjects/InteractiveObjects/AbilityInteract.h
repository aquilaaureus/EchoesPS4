// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Includes
#include "CoreMinimal.h"
#include "Actors/EnvironmentObjects/InteractiveObjects/InteractableObject.h"
#include "Actors/Kira/PlayerAbilities.h"
#include "AbilityInteract.generated.h"

// Fwd dcl.
class AMenuWidgetPlayerController;
class UBoxComponent;
class UChestAnimInstance;
class USkeletalMeshComponent;
class USoundCue;
class UUserWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FChestOpenedDelegate);


//////////////////////////////////////////////////////////////////////////
// Class AAbilityInteract: A specific type of interactable object within the game
// that will reward the player with a new ability. The ability to give the player
// is set on an instance, of a derived blueprint class based on this object. It
// uses an enum to track all of the different abilities that the player can be
// rewarded with.
//////////////////////////////////////////////////////////////////////////
UCLASS()
class ECHOES_API AAbilityInteract : public AInteractableObject
{
	GENERATED_BODY()
	
private:
	// The angle of ACos that the player needs to be above in order to interact with the Ability Mesh 
	UPROPERTY( EditAnywhere, Category = "Designer Values", DisplayName = "Angle to Look at" )
	float fAngleToLookAt;

	// Pointer to the character 
	UPROPERTY()
	AEchoesCharacter* m_pcEchoesCharacter;

	// The animation instance used to control chest animations
	UPROPERTY( Transient )
	UChestAnimInstance* m_pcChestAnimation;

	// The actual instance of the userwidget created and added to viewport
	UPROPERTY( Transient )
	UUserWidget* m_pcAbilityWidgetInstance;

	// A variable for holding the flinch sounds
	UPROPERTY ( EditAnywhere , Category = "Sound" , DisplayName = "Ability Popup Sound" )
	USoundCue* m_pcAbilityPopupSound;

	// Pointer to the player controller
	AMenuWidgetPlayerController* m_pcPlayerController;

	// Use this to get the Forward Vector of this Actor
	FVector m_sAbilityInteractForwardVector;

	// Callback function to close the popup that has been opened
	UFUNCTION()
	void CBClosePopup();

protected:
	// The skeletal mesh used for the animation
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Skeletal Mesh" )
	USkeletalMeshComponent* m_pcSkeletalMeshComponent;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FChestOpenedDelegate m_dChestOpenedDelegate;

	// The Box Mesh used for the animation 
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Interaction Box" )
	UBoxComponent*	m_pcBoxColliderComponent;

	// The sound effect to be played continuously while the chest is active.
	UPROPERTY( EditDefaultsOnly, Category = "Designer Values", DisplayName = "Idle SFX" )
	USoundCue* m_pcIdleSfx;

	// The sound to be played when the chest is opened
	UPROPERTY( EditDefaultsOnly, Category = "Designer Values", DisplayName = "Activation SFX" )
	USoundCue* m_pcActivationSfx;

	// The ability popup widget that will be displayed on the screen
	UPROPERTY( EditAnywhere, Category = "Designer Values", DisplayName = "Ability Popup" )
	TSubclassOf<UUserWidget> m_pcAbilityWidget;

	// Perform the appropriate removal of Interaction Functionality 
	void RemoveInteraction();

	// Called when object spawns or play starts
	virtual void BeginPlay() override;

	// Tick Func called every frame
	virtual void Tick( float fInDeltaTime ) override; 

public:
	// The ability that the player will get from interacting with this object
	UPROPERTY( EditAnywhere, Category = "Designer Values", DisplayName = "Ability Gain" )
	EAbility m_eAbilityPickup;

	// Sets default values
	AAbilityInteract();

	// Callback function for when the player interacts with the object
	virtual void CBInteract() override;
};
