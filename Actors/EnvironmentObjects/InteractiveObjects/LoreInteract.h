// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Includes
#include "CoreMinimal.h"
#include "Actors/EnvironmentObjects/InteractiveObjects/InteractableObject.h"
#include "LoreInteract.generated.h"

//////////////////////////////////////////////////////////////////////////
// Class ALoreInteract: This is a specific type of interactable object that can
// be placed within our game. It reads an xml file for a lore item that is specified
// on an instance and then displays it in the log (UI work to be done). For it to be
// used a Blueprint subclass needs to be made to set the static mesh of the object,
// however the lore reference ID should be set on an instance within the scene.
//////////////////////////////////////////////////////////////////////////
UCLASS()
class ECHOES_API ALoreInteract : public AInteractableObject
{
	GENERATED_BODY()

public:
	// The ability that the player will get from interacting with this object
	UPROPERTY( EditAnywhere, Category = "Designer Values", DisplayName = "Codex Entry Category" )
	FString m_strCodexEntryCategory;

	// The ability that the player will get from interacting with this object
	UPROPERTY( EditAnywhere, Category = "Designer Values", DisplayName = "Codex Entry Name" )
	FString m_strCodexEntryName;

	// Callback function for when the player interacts with the object
	virtual void CBInteract() override;
};
