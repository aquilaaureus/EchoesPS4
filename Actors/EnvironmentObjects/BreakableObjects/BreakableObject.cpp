// Fill out your copyright notice in the Description page of Project Settings.

// Header Include
#include "BreakableObject.h"

// Other includes
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Engine/World.h"


//////////////////////////////////////////////////////////////////////////
// Sets default values
//////////////////////////////////////////////////////////////////////////
ABreakableObject::ABreakableObject()
{
	// Add the tag so the player can damage these objects
	m_pcStaticMeshComponent->ComponentTags.Add( FName( "PlayerDamageable" ) );
}

//////////////////////////////////////////////////////////////////////////
// A function UE4 has to control the damage the object will take from the character
//////////////////////////////////////////////////////////////////////////
float ABreakableObject::TakeDamage( float fDamageAmount, FDamageEvent const& krsDamageEvent,
	AController* pcEventInstigator, AActor* pcDamageCauser )
{
	//Broadcast for vfx events
	if( m_dObjectBroken.IsBound() )
	{
		m_dObjectBroken.Broadcast();
	}

	// Destroy the pot from the world
	Destroy();

	// Play a sound and fire delegate when the breakable is destroyed
	if( m_apcBreakingSounds.Num() > 0 )
	{
		UGameplayStatics::PlaySoundAtLocation( this, m_apcBreakingSounds[ FMath::RandRange( 0, m_apcBreakingSounds.Num() - 1 ) ], GetActorLocation() );
	}
	
	return fDamageAmount;
}

void ABreakableObject::BeginPlay()
{
	OnEndPlay.AddDynamic(this, &ABreakableObject::ReplaceWithBroken);

	Super::BeginPlay();
}

void ABreakableObject::ReplaceWithBroken(AActor * pcActor, EEndPlayReason::Type EReason)
{
	if (EReason != EEndPlayReason::Destroyed)
	{
		return; //Do nothing unless the function was called because of Destroy()
	}

	if (m_pcBrokenActorsArray.Num() == 0)
	{
		return; //Don't continue if the array is empty
	}

	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	TSubclassOf<AActor> pcActorToSpawn;
	if (m_pcBrokenActorsArray.Num() == 1) //only one element in the array
	{
		pcActorToSpawn = m_pcBrokenActorsArray.GetData()[0];
	}
	else
	{
		int i = FMath::RandRange(0, m_pcBrokenActorsArray.Num() - 1);
		pcActorToSpawn = m_pcBrokenActorsArray.GetData()[i];
	}

	if (pcActorToSpawn)
	{
		GetWorld()->SpawnActor<AActor>(pcActorToSpawn, GetActorTransform(), params);
	}
}

