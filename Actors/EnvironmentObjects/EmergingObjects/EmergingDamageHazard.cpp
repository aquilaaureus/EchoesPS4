// Fill out your copyright notice in the Description page of Project Settings.

// Header include 
#include "EmergingDamageHazard.h"

// Other includes
#include "Actors/Kira/EchoesCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "../ParticleHelper.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Sound/SoundCue.h"
#include "TimerManager.h"

//////////////////////////////////////////////////////////////////////////
// Sets default values
//////////////////////////////////////////////////////////////////////////
AEmergingDamageHazard::AEmergingDamageHazard()
: m_pcOverlappingActor			( nullptr )
, m_fSinkingMultiplier			( 0.f )
, m_bIsSinking					( false )
, m_pcForceKnockbackUpper		( nullptr )
, m_pcForceKnockbackLower		( nullptr )
, m_fKnockbackForce				( 5000.f )
, m_fKnockbackRadius			( 100.f )
, m_pcDerbisPartSys				( nullptr )
, m_pcSpawnedParticleSystem		( nullptr )
, m_pcEmergeSound				( nullptr )
, m_pcRetractSound				( nullptr )
, m_fSinkTime					( 1.0f )
, m_fActiveTime					( 5.0f )
, m_fDamageDealt				( 10.f )
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the static mesh component
	m_pcStaticMeshComponent->SetCollisionResponseToAllChannels( ECollisionResponse::ECR_Overlap );

	// Create the force knockback component
	m_pcForceKnockbackUpper = CreateDefaultSubobject<URadialForceComponent>( TEXT( "ForceKnockbackUpper" ) );
	m_pcForceKnockbackUpper->AttachToComponent( RootComponent, FAttachmentTransformRules::KeepRelativeTransform );

	m_pcForceKnockbackLower = CreateDefaultSubobject<URadialForceComponent>( TEXT( "ForceKnockbackLower" ) );
	m_pcForceKnockbackLower->AttachToComponent( RootComponent, FAttachmentTransformRules::KeepRelativeTransform );
}

//////////////////////////////////////////////////////////////////////////
// Called when the game starts or when spawned
//////////////////////////////////////////////////////////////////////////
void AEmergingDamageHazard::BeginPlay()
{
	Super::BeginPlay();

	// Calculate time multipliers for emerging/sinking
	m_fSinkingMultiplier = 1.0f / m_fSinkTime;

	if( !m_pcEmergeSound ) { UE_LOG( LogTemp, Error, TEXT( "Member variable Emerge Sound not set on %s" ), *GetName() ); }
	if( !m_pcRetractSound ) { UE_LOG( LogTemp, Error, TEXT( "Member variable Retract Sound not set on %s" ), *GetName() ); }

	// Calculate/Set values from the editor
	m_pcForceKnockbackUpper->ImpulseStrength = m_fKnockbackForce;
	m_pcForceKnockbackLower->ImpulseStrength = m_fKnockbackForce;

	m_pcForceKnockbackUpper->Radius = m_fKnockbackRadius;
	m_pcForceKnockbackLower->Radius = m_fKnockbackRadius;
}

//////////////////////////////////////////////////////////////////////////
// Called every frame
//////////////////////////////////////////////////////////////////////////
void AEmergingDamageHazard::Tick( float fDeltaTime )
{
	Super::Tick( fDeltaTime);

	if( m_bIsSinking )
	{
		// Interpolate the crystal based on set values and delatTime
		float fTimeToAdd = fDeltaTime * m_fSinkingMultiplier;
		m_fCurrentInterpolation += fTimeToAdd;
		SetActorRelativeLocation( FMath::Lerp<FVector>( GetEmergeTargetLocation(), GetEmergeStartLocation(), m_fCurrentInterpolation ) );
		m_pcDecalComponent->SetWorldLocation( m_sDecalWorldLocation );

		// Stop interpolating if target position has been reached
		if( m_fCurrentInterpolation > 1.0f )
		{
			m_bIsSinking = false;
			m_fCurrentInterpolation = 0.f;

			// Add delegates for when the player enters the collision box
			m_pcCollisionBox->OnComponentBeginOverlap.AddDynamic( this, &AEmergingDamageHazard::CBOnOverlapBegin );

			// Turn the crystal invisible so it cannot be seen through the floor
			m_pcStaticMeshComponent->SetVisibility( false );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Function to be overwritten in children to control behaviour for after the hazard has emerged
//////////////////////////////////////////////////////////////////////////
void AEmergingDamageHazard::EmergeCompleted()
{
	FTimerHandle sTimer;
	GetWorld()->GetTimerManager().SetTimer( sTimer, this, &AEmergingDamageHazard::CBActiveTimeEnded, m_fActiveTime, false );

	// If the player is still on the crystal, apply damage
	if( m_pcStaticMeshComponent->IsOverlappingActor( m_pcOverlappingActor ) )
	{
		UGameplayStatics::ApplyDamage( m_pcOverlappingActor, m_fDamageDealt, GetInstigatorController(), this, UDamageType::StaticClass() );

		m_pcForceKnockbackUpper->FireImpulse();
		m_pcForceKnockbackLower->FireImpulse();
	}
}

//////////////////////////////////////////////////////////////////////////
// Delegate function to be called when player overlaps the collider
//////////////////////////////////////////////////////////////////////////
void AEmergingDamageHazard::CBOnOverlapBegin( UPrimitiveComponent* pcHitComp, AActor* pcOtherActor, UPrimitiveComponent* pcOtherComp,
	int32 iOtherBodyIndex, bool bFromSweep, const FHitResult& rsSweepResult )
{
	AEmergingHazard::CBOnOverlapBegin( pcHitComp, pcOtherActor, pcOtherComp, iOtherBodyIndex, bFromSweep, rsSweepResult );

	// If the player enters the collision box, then project the crystal out of the ground
	if( Cast<AEchoesCharacter>( pcOtherActor ) )
	{
		m_pcOverlappingActor = pcOtherActor;

		UGameplayStatics::PlaySoundAtLocation( this, m_pcEmergeSound, GetActorLocation() );

		if( m_pcSpawnedParticleSystem )
		{
			m_pcSpawnedParticleSystem->Activate( true );
		}
		else
		{
			m_pcSpawnedParticleSystem = UGameplayStatics::SpawnEmitterAtLocation( this, m_pcDerbisPartSys, (GetActorLocation() + FVector( 0.f, 0.f, 30.0f )), FRotator::ZeroRotator, false );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Callback function to trigger the crystal sinking beneath the ground
//////////////////////////////////////////////////////////////////////////
void AEmergingDamageHazard::CBActiveTimeEnded()
{
	// Start the crystal sinking
	m_bIsSinking = true;
	UGameplayStatics::PlaySoundAtLocation( this, m_pcRetractSound, GetActorLocation() );
}