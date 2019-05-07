// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Includes
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnvironmentObject.generated.h"

// Fwd dcl.
class UStaticMeshComponent;

//////////////////////////////////////////////////////////////////////////
// Class AEnvironmentObject: The base class that is used for any kind of environment 
// object within the game that has an active effect and is not just scenery. As this
// class only has behaviour and data common to all environment objects it only has a 
// static mesh component.
//////////////////////////////////////////////////////////////////////////
UCLASS()
class ECHOES_API AEnvironmentObject : public AActor
{
	GENERATED_BODY()
	
protected:
	// The static mesh component of this environment object
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "StaticMesh" )
	UStaticMeshComponent* m_pcStaticMeshComponent;

public:	
	// Sets default values for this actor's properties
	AEnvironmentObject();

};
