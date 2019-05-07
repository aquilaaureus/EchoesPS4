// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FootprintComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECHOES_API UFootprintComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFootprintComponent();

	// Creates a footprint particle at given location with given rotation
	void LeaveFootprint( FVector vLocation, FRotator vRotation, UPhysicalMaterial* material );

private:

	UPROPERTY( EditAnywhere, Category = "VFX" )
	class UParticleSystem*  m_pcFootprints;
	
	UPROPERTY( EditAnywhere, Category = "VFX" )
	class UParticleSystem* m_pcDustCloud;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;
		
	
};
