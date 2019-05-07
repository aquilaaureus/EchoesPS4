// Fill out your copyright notice in the Description page of Project Settings.

// Header include
#include "Pickup.h"

// Other includes
#include "Actors/Kira/EchoesCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerComponents/PlayerStatsComponent.h"
#include "TimerManager.h"

//////////////////////////////////////////////////////////////////////////
// Sets default values
//////////////////////////////////////////////////////////////////////////
APickup::APickup()
: m_bIsActive			( false )
, m_pcStaticMeshComp	( nullptr )
, m_pcSphereComponent	( nullptr )
, m_pcPlayer			( nullptr )
, m_pcPlayerStatsComp	( nullptr )
, m_fSpeed				( 1000.f )
, m_fDisappearDistance	( 100.f )
, m_fMagnetismRadius	( 1000.f )
, m_fDelay				( 1.f )
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set up the collision for the item
	m_pcStaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>( FName( "CollisionMesh" ) );
	SetRootComponent( m_pcStaticMeshComp );
	m_pcStaticMeshComp->SetSimulatePhysics( true );

	m_pcStaticMeshComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	// Create the magnetism collision area
	m_pcSphereComponent = CreateDefaultSubobject<USphereComponent>( FName( "SphereCollision" ) );
	m_pcSphereComponent->AttachToComponent( RootComponent, FAttachmentTransformRules::KeepRelativeTransform );
}

//////////////////////////////////////////////////////////////////////////
// Called when the game starts or when spawned
//////////////////////////////////////////////////////////////////////////
void APickup::BeginPlay()
{
	Super::BeginPlay();

	// Find the player character
	m_pcPlayer = Cast<AEchoesCharacter>( UGameplayStatics::GetPlayerCharacter( this, 0 ) );
	
	if( !IsValid( m_pcPlayer ) )
	{
		UE_LOG( LogTemp, Error, TEXT( "Player cannot be found on object %s" ), *GetName() );
		Destroy(); //Do not allow to exist without a player
	}

	// Set editor values
	m_pcSphereComponent->SetSphereRadius( m_fMagnetismRadius );

	// Delay activation of the pickup for a short time
	ToggleActive( false );

	FTimerHandle sTimerHandle;
	GetWorld()->GetTimerManager().SetTimer( sTimerHandle, this, &APickup::CBCheckActivation, m_fDelay, false );
	
	//Set collision to only respond to overlapping pawn and ignore all other channels.
	FCollisionResponseContainer sCollisionResponses = FCollisionResponseContainer();
	sCollisionResponses.SetAllChannels( ECollisionResponse::ECR_Ignore );
	sCollisionResponses.SetResponse( ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap );
	m_pcSphereComponent->SetCollisionResponseToChannels( sCollisionResponses );

	// Bind the overlap event of the sphere
	m_pcSphereComponent->OnComponentBeginOverlap.AddDynamic( this, &APickup::CBOnOverlapBegin );

	// Find the player stats component
	if( m_pcPlayer )
	{
		m_pcPlayerStatsComp = Cast<UPlayerStatsComponent>( m_pcPlayer->GetComponentByClass( UPlayerStatsComponent::StaticClass() ) );

		if( !m_pcPlayerStatsComp )
		{
			UE_LOG( LogTemp, Error, TEXT( "Player Stats Component not found on %s" ), *GetName() );
			Destroy(); //Do not allow to exist without a player stats component
		}
	}
	
}

//////////////////////////////////////////////////////////////////////////
// Called every frame
//////////////////////////////////////////////////////////////////////////
void APickup::Tick( float fDeltaTime )
{
	Super::Tick( fDeltaTime );

	if( m_bIsActive )
	{
		if( m_pcPlayer )
		{
			// if the player is overlapping with the pickup's magnet field
			if( IsOverlappingActor( m_pcPlayer ) )
			{
				// make the pickup move towards the player
				FVector vDirection = m_pcPlayer->GetActorLocation() - GetActorLocation();

				// if that distance is less than the value of m_fDisappearDistance
				if( vDirection.Size() <= m_fDisappearDistance )
				{
					PickupCollected();
				}
				// else if that distance is greater
				else
				{
					// make the pickup move towards the player 
					vDirection.Normalize();
					vDirection *= m_fSpeed * fDeltaTime;
					SetActorLocation( GetActorLocation() + vDirection );
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Toggles the object on and off based on the parameter
//////////////////////////////////////////////////////////////////////////
void APickup::ToggleActive( bool bActive )
{
	m_bIsActive = bActive;

	// Don't simulate physics while flying toward the player
	if( m_bIsActive )
	{
		m_pcStaticMeshComp->SetSimulatePhysics( false );
	}
	else
	{
		m_pcStaticMeshComp->SetPhysicsLinearVelocity( FVector( 0.f, 0.f, 0.f ) );
		m_pcStaticMeshComp->SetSimulatePhysics( true );
	}
}

//////////////////////////////////////////////////////////////////////////
// Delegate function to be called when player overlaps the collider
//////////////////////////////////////////////////////////////////////////
void APickup::CBOnOverlapBegin( UPrimitiveComponent* pcHitComp, AActor* pcOtherActor, UPrimitiveComponent* pcOtherComp,
	int32 iOtherBodyIndex, bool bFromSweep, const FHitResult& rsSweepResult )
{
	if( ShouldBeCollected() )
	{
		ToggleActive( true );
		m_pcSphereComponent->OnComponentBeginOverlap.RemoveAll( this );
	}
}

//////////////////////////////////////////////////////////////////////////
// Check to see if the object should be activated or not
//////////////////////////////////////////////////////////////////////////
void APickup::CBCheckActivation()
{
	// If the player is not overlapping, leave active as false
	if( m_pcPlayer )
	{
		if( IsOverlappingActor( m_pcPlayer ) && ShouldBeCollected() )
		{
			ToggleActive( true );
		}
		else
		{
			ToggleActive( false );
		}
	}
	else
	{
		UE_LOG( LogTemp, Error, TEXT( "No player found for object: %s" ), *GetName() );
	}
}