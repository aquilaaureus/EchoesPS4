// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GhostSpawnComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECHOES_API UGhostSpawnComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGhostSpawnComponent();

	// Creates a footprint particle at given location with given rotation
	void SpawnGhost( FVector vLocation, FRotator vRotation );

private:

	UPROPERTY( EditAnywhere, Category = "GhostMesh" )
	UBlueprint* m_pcGhost;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
};
