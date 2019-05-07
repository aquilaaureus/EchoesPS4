// Fill out your copyright notice in the Description page of Project Settings.

// Header include
#include "EmergingHazard.h"

// Other includes
#include "Actors/Kira/EchoesCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"

//////////////////////////////////////////////////////////////////////////
// Set default values
//////////////////////////////////////////////////////////////////////////
AEmergingHazard::AEmergingHazard()
: m_sStartLocation					( FVector::ZeroVector )
, m_sTargetLocation					( FVector::ZeroVector )
, m_fCollisionBoxHeight				( 100.f )
, m_fInterpolationMultiplier		( 0.f )
, m_bIsEmerging						( false )
, m_fDecalRandomMultiplierMinBound	( 0.75f )
, m_fDecalRandomMultiplierMaxBound	( 0.9f )
, m_sDecalWorldLocation				( FVector::ZeroVector )
, m_pcCollisionBox					( nullptr )
, m_fDelayTime						( 0.f )
, m_fEmergeTime						( 0.1f )
, m_fCurrentInterpolation			( 0.f )
{
	// Set Tick to true
	PrimaryActorTick.bCanEverTick = true;

	// Create the collider to sit above the mesh
	m_pcCollisionBox = CreateDefaultSubobject<UBoxComponent>( TEXT( "CollisionBox" ) );
	m_pcCollisionBox->AttachToComponent( RootComponent, FAttachmentTransformRules::KeepRelativeTransform );

	//Set collision to only respond to overlapping pawn and ignore all other channels.
	FCollisionResponseContainer sCollisionResponses = FCollisionResponseContainer();
	sCollisionResponses.SetAllChannels( ECollisionResponse::ECR_Ignore );
	sCollisionResponses.SetResponse( ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap );
	m_pcCollisionBox->SetCollisionResponseToChannels( sCollisionResponses );

	// Add delegates for when the player enters the collision box
	m_pcCollisionBox->OnComponentBeginOverlap.AddDynamic( this, &AEmergingHazard::CBOnOverlapBegin );

	// Create the decal component used to project decals
	m_pcDecalComponent = CreateDefaultSubobject<UDecalComponent>( TEXT( "DecalComponent" ) );
	m_pcDecalComponent->AttachToComponent( RootComponent, FAttachmentTransformRules::KeepRelativeTransform );

	m_pcDecalComponent->SetVisibility( false );
}

//////////////////////////////////////////////////////////////////////////
// Called when the game starts or when spawned
//////////////////////////////////////////////////////////////////////////
void AEmergingHazard::BeginPlay()
{
	Super::BeginPlay();

	if( m_pcStaticMeshComponent->GetStaticMesh() )
	{
		// Get the size of the mesh used for location/size calculations
		FVector sMeshSize = m_pcStaticMeshComponent->GetStaticMesh()->GetBoundingBox().GetSize();

		// Store current and target positions
		m_sStartLocation = GetActorLocation();
		m_sTargetLocation = FVector( m_sStartLocation.X, m_sStartLocation.Y, m_sStartLocation.Z + sMeshSize.Z );
	
		// Random float roation values for Z axis rotation of Actor and Decal Component
		float fHazardRotation = UKismetMathLibrary::RandomFloatInRange( 0.f, 360.f );

		// Set a random rotation for the Emerging Hazard Mesh Component
		// FRotator values in the following order inPitch, inYaw, inRoll ( Y, Z, X ) 
		m_pcStaticMeshComponent->SetRelativeRotation( FRotator( m_pcStaticMeshComponent->GetComponentRotation().Pitch, fHazardRotation, m_pcStaticMeshComponent->GetComponentRotation().Roll ) );

		float fDecalRotation = UKismetMathLibrary::RandomFloatInRange( 0.f, 360.f );

		float fDecalRandomMultiplierY = UKismetMathLibrary::RandomFloatInRange( m_fDecalRandomMultiplierMinBound, m_fDecalRandomMultiplierMaxBound );
		float fDecalRandomMultiplierZ = UKismetMathLibrary::RandomFloatInRange( m_fDecalRandomMultiplierMinBound, m_fDecalRandomMultiplierMaxBound );

		// Set the decal component just above and over the whole mesh
		m_pcDecalComponent->DecalSize = FVector( 20.f, sMeshSize.Y, sMeshSize.X );
		m_pcDecalComponent->SetRelativeScale3D( FVector( m_pcDecalComponent->GetComponentScale().X, fDecalRandomMultiplierY, fDecalRandomMultiplierZ ) );
		m_pcDecalComponent->SetVisibility( true );
		
		// Set a random roation for the Decal Component independant from the Actor Rotation
		// Set the Location of the Decal in World Space so that it always stays attached to the floor
		m_pcDecalComponent->SetWorldRotation( FRotator( 90.f, 0.f, fDecalRotation ) );
		m_pcDecalComponent->SetWorldLocation( GetActorLocation() + FVector( 0.f, 0.f, ( sMeshSize.Z * 0.5f ) ) );

		// Set this variable after it's World position has been established
		m_sDecalWorldLocation = m_pcDecalComponent->GetComponentLocation();

		// Positions the box above the crystal
		m_pcCollisionBox->SetRelativeLocation( FVector( 0.f, 0.f, sMeshSize.Z + m_fCollisionBoxHeight ) );
		sMeshSize *= 0.25f; // Creates the box length/width to be just over the center to the crystal
		m_pcCollisionBox->SetBoxExtent( FVector( sMeshSize.X, sMeshSize.Y, m_fCollisionBoxHeight ) );
	}

	// Set invisible until activated
	m_pcStaticMeshComponent->SetVisibility( false );

	// Calculate interpolation multiplier
	m_fInterpolationMultiplier = 1.f / m_fEmergeTime;
}

//////////////////////////////////////////////////////////////////////////
// Called every frame
//////////////////////////////////////////////////////////////////////////
void AEmergingHazard::Tick( float fDeltaTime )
{
	Super::Tick( fDeltaTime );

	if( m_bIsEmerging )
	{
		// Interpolate the crystal based on set values and delatTime
		float fTimeToAdd = fDeltaTime * m_fInterpolationMultiplier;
		m_fCurrentInterpolation += fTimeToAdd;
		SetActorRelativeLocation( FMath::Lerp<FVector>( m_sStartLocation, m_sTargetLocation, m_fCurrentInterpolation ) );
		m_pcDecalComponent->SetWorldLocation( m_sDecalWorldLocation );

		// Stop interpolating if target position has been reached
		if( m_fCurrentInterpolation > 1.0f )
		{
			m_bIsEmerging = false;
			m_fCurrentInterpolation = 0.f;
			EmergeCompleted();
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Delegate function to be called when player overlaps the collider
//////////////////////////////////////////////////////////////////////////
void AEmergingHazard::CBOnOverlapBegin( UPrimitiveComponent* pcHitComp, AActor* pcOtherActor, UPrimitiveComponent* pcOtherComp,
	int32 iOtherBodyIndex, bool bFromSweep, const FHitResult& rsSweepResult )
{
	if( Cast<AEchoesCharacter>( pcOtherActor ) )
	{
		// Remove the delegate so that this function is only called the first time the player enters
		m_pcCollisionBox->OnComponentBeginOverlap.RemoveDynamic( this, &AEmergingHazard::CBOnOverlapBegin );

		// Set the mesh visible
		m_pcStaticMeshComponent->SetVisibility( true );

		// Check if a delay is required before emerging the hazard
		if( m_fDelayTime > 0.f )
		{
			// Create a delay timer before triggering the emerging of the hazard
			FTimerHandle sTimer;
			GetWorld()->GetTimerManager().SetTimer( sTimer, this, &AEmergingHazard::CBEmergeDelayEnded, m_fDelayTime, false );
		}
		else
		{
			CBEmergeDelayEnded();
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Callback function to trigger the crystal exploding from the ground
//////////////////////////////////////////////////////////////////////////
void AEmergingHazard::CBEmergeDelayEnded()
{
	// Start the crystal emerging
	m_bIsEmerging = true;
}