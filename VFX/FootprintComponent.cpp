// Fill out your copyright notice in the Description page of Project Settings.

#include "FootprintComponent.h"
#include "Kismet/GameplayStatics.h"
#include <Particles/ParticleSystem.h>
#include <Particles/ParticleSystemComponent.h>
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/PhysicalMaterials/PhysicalMaterial.h"

// Sets default values for this component's properties
UFootprintComponent::UFootprintComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFootprintComponent::BeginPlay()
{
	Super::BeginPlay();

}


void UFootprintComponent::LeaveFootprint( FVector vLocation, FRotator vRotation, UPhysicalMaterial* material )
{
	//snow only exists at the start of the level. Later we will use physical materials to use this
	//Use multiple particle systems for the different effects. Also trigger sound effects here.
	//Use the physics material to switch on the exact effect+sound to activate.

	EPhysicalSurface surface = UPhysicalMaterial::DetermineSurfaceType( material );

	switch (surface)
	{
	case SurfaceType_Default:
		break;
	case SurfaceType1: //snow
		if (m_pcFootprints)
		{
			UGameplayStatics::SpawnEmitterAtLocation( GetWorld(), m_pcFootprints, vLocation, FRotator::ZeroRotator, true );
		}
		break;
	case SurfaceType2: //dirt
		if (m_pcDustCloud)
		{
			UGameplayStatics::SpawnEmitterAtLocation( GetWorld(), m_pcDustCloud, vLocation, FRotator::ZeroRotator, true );
		}
		break;
	case SurfaceType3:
		break;
	case SurfaceType4:
		break;
	case SurfaceType5:
		break;
	case SurfaceType6:
		break;
	case SurfaceType7:
		break;
	case SurfaceType8:
		break;
	case SurfaceType9:
		break;
	case SurfaceType10:
		break;
	case SurfaceType11:
		break;
	case SurfaceType12:
		break;
	case SurfaceType13:
		break;
	case SurfaceType14:
		break;
	case SurfaceType15:
		break;
	case SurfaceType16:
		break;
	case SurfaceType17:
		break;
	case SurfaceType18:
		break;
	case SurfaceType19:
		break;
	case SurfaceType20:
		break;
	case SurfaceType21:
		break;
	case SurfaceType22:
		break;
	case SurfaceType23:
		break;
	case SurfaceType24:
		break;
	case SurfaceType25:
		break;
	case SurfaceType26:
		break;
	case SurfaceType27:
		break;
	case SurfaceType28:
		break;
	case SurfaceType29:
		break;
	case SurfaceType30:
		break;
	case SurfaceType31:
		break;
	case SurfaceType32:
		break;
	case SurfaceType33:
		break;
	case SurfaceType34:
		break;
	case SurfaceType35:
		break;
	case SurfaceType36:
		break;
	case SurfaceType37:
		break;
	case SurfaceType38:
		break;
	case SurfaceType39:
		break;
	case SurfaceType40:
		break;
	case SurfaceType41:
		break;
	case SurfaceType42:
		break;
	case SurfaceType43:
		break;
	case SurfaceType44:
		break;
	case SurfaceType45:
		break;
	case SurfaceType46:
		break;
	case SurfaceType47:
		break;
	case SurfaceType48:
		break;
	case SurfaceType49:
		break;
	case SurfaceType50:
		break;
	case SurfaceType51:
		break;
	case SurfaceType52:
		break;
	case SurfaceType53:
		break;
	case SurfaceType54:
		break;
	case SurfaceType55:
		break;
	case SurfaceType56:
		break;
	case SurfaceType57:
		break;
	case SurfaceType58:
		break;
	case SurfaceType59:
		break;
	case SurfaceType60:
		break;
	case SurfaceType61:
		break;
	case SurfaceType62:
		break;
	case SurfaceType_Max:
		break;
	default:
		break;
	}

}
