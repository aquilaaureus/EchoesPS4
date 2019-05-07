// Fill out your copyright notice in the Description page of Project Settings.

// Header include
#include "CorruptionAura.h"

// Other includes
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Actors/Kira/EchoesCharacter.h"
#include "PlayerComponents/PlayerStatsComponent.h"

//////////////////////////////////////////////////////////////////////////
// Sets default values
//////////////////////////////////////////////////////////////////////////
ACorruptionAura::ACorruptionAura()
: m_pcPlayerCharacter		( nullptr )
, m_fElapsedCorruptionTime	( 0.f )
, m_fTickTime				( 2.f )
, m_fDamage					( 5.f )
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the collider to deal corruption damage
	m_pcCollisionBox = CreateDefaultSubobject<UBoxComponent>( TEXT( "CollisionBox" ) );
	SetRootComponent( m_pcCollisionBox );

	// Set collision to only respond to overlapping pawn and ignore all other channels.
	FCollisionResponseContainer sCollisionResponses = FCollisionResponseContainer();
	sCollisionResponses.SetAllChannels( ECollisionResponse::ECR_Ignore );
	sCollisionResponses.SetResponse( ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap );
	m_pcCollisionBox->SetCollisionResponseToChannels( sCollisionResponses );

	// Add delegates for when the player enters the collision box
	m_pcCollisionBox->OnComponentBeginOverlap.AddDynamic( this, &ACorruptionAura::CBOnOverlapBegin );
	m_pcCollisionBox->OnComponentEndOverlap.AddDynamic( this, &ACorruptionAura::CBOnOverlapEnd );
}

//////////////////////////////////////////////////////////////////////////
// Called once per frame as the game loops
//////////////////////////////////////////////////////////////////////////
void ACorruptionAura::Tick( float fDeltaTime )
{
	Super::Tick( fDeltaTime );

	// If the player is in the corruption area
	if( nullptr != m_pcPlayerCharacter )
	{
		// Double check the overlapping
		if( m_pcCollisionBox->IsOverlappingActor( m_pcPlayerCharacter ) )
		{
			// Set the Overlap to True - Will stop invulnerability
			m_pcPlayerCharacter->m_pcPlayerStatsComponent->SetCorruptionOverlapping( true );

			// Increase elapsed time and check for damage
			m_fElapsedCorruptionTime += fDeltaTime;

			if( m_fElapsedCorruptionTime > m_fTickTime )
			{
				UGameplayStatics::ApplyDamage( m_pcPlayerCharacter, m_fDamage, GetInstigatorController(), this, UDamageType::StaticClass() );
				m_fElapsedCorruptionTime = 0.f;
			}
		}
		else
		{
			// Set the Overlap to False - Allows other damage sources to begin invulnerability on the player
			m_pcPlayerCharacter->m_pcPlayerStatsComponent->SetCorruptionOverlapping( false );
			m_fElapsedCorruptionTime = 0.f;
		}
	}
	else
	{
		m_fElapsedCorruptionTime = 0.f;
	}
}

//////////////////////////////////////////////////////////////////////////
// Delegate function to be called when player overlaps the collider
//////////////////////////////////////////////////////////////////////////
void ACorruptionAura::CBOnOverlapBegin( UPrimitiveComponent* pcHitComp, AActor* pcOtherActor, UPrimitiveComponent* pcOtherComp,
	int32 iOtherBodyIndex, bool bFromSweep, const FHitResult& rsSweepResult )
{
	// If the overlapping Pawn is the player, store a reference to it
	AEchoesCharacter* pcCharacter = Cast<AEchoesCharacter>( pcOtherActor );

	if( pcCharacter )
	{
		m_pcPlayerCharacter = pcCharacter;
	}
}

//////////////////////////////////////////////////////////////////////////
// Delegate function to be called when the player leaves the collider
//////////////////////////////////////////////////////////////////////////
void ACorruptionAura::CBOnOverlapEnd( UPrimitiveComponent* pcOverlappedComp, AActor* pcOtherActor, UPrimitiveComponent* pcOtherComp,
	int32 iOtherBodyIndex )
{
	// If the overlapping Pawn is the player, store a reference to it
	AEchoesCharacter* pcCharacter = Cast<AEchoesCharacter>( pcOtherActor );

	if( pcCharacter )
	{
		m_pcPlayerCharacter = nullptr;
	}
}