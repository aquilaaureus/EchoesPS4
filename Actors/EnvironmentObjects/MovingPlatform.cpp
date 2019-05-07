// Fill out your copyright notice in the Description page of Project Settings.

// Header includes
#include "MovingPlatform.h"

// Other includes
#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

//////////////////////////////////////////////////////////////////////////
// Sets default values
//////////////////////////////////////////////////////////////////////////
AMovingPlatform::AMovingPlatform()
: m_sStartLocation			( FVector::ZeroVector )
, m_sTargetLocation			( FVector::ZeroVector )
, m_eCurrentStatus			( EMovingStatus::EMovingStatus_Forwards )
, m_eNextState				( EMovingStatus::EMovingStatus_Forwards )
, m_fCurrentInterpolation	( 0.f )
, m_fMovementMultiplier		( 0.f )
, m_fHaltTime				( 2.f )
, m_fMoveTime				( 1.f )
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the component whose location will be used for target location
	m_pcTargetLocation = CreateDefaultSubobject<USphereComponent>( TEXT( "CollisionBox" ) );
	m_pcTargetLocation->AttachToComponent( RootComponent, FAttachmentTransformRules::KeepRelativeTransform );

	// Set collision to ignore everything as only its location is needed not the component itself
	FCollisionResponseContainer sCollisionResponses = FCollisionResponseContainer();
	sCollisionResponses.SetAllChannels( ECollisionResponse::ECR_Ignore );
	m_pcTargetLocation->SetCollisionResponseToChannels( sCollisionResponses );
}

//////////////////////////////////////////////////////////////////////////
// Called when the game starts or when spawned
//////////////////////////////////////////////////////////////////////////
void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	// Calculate member data from editor values
	m_sStartLocation = GetActorLocation();
	m_sTargetLocation = m_sStartLocation + m_pcTargetLocation->GetRelativeTransform().GetLocation();
	m_fMovementMultiplier = 1.f / m_fMoveTime;
}

//////////////////////////////////////////////////////////////////////////
// Called every frame
//////////////////////////////////////////////////////////////////////////
void AMovingPlatform::Tick( float fDeltaTime )
{
	Super::Tick( fDeltaTime );

	// Set interpolation amount
	float fTimeToAdd = fDeltaTime * m_fMovementMultiplier;
	m_fCurrentInterpolation += fTimeToAdd;

	// Move the platform in the right direction
	switch( m_eCurrentStatus )
	{
		case EMovingStatus::EMovingStatus_Forwards:
			SetActorRelativeLocation( FMath::Lerp<FVector>( m_sStartLocation, m_sTargetLocation, m_fCurrentInterpolation ) );
			break;
		case EMovingStatus::EMovingStatus_Backwards:
			SetActorRelativeLocation( FMath::Lerp<FVector>( m_sTargetLocation, m_sStartLocation, m_fCurrentInterpolation ) );
			break;
		case EMovingStatus::EMovingStatus_Halted:
			break;
		default:
			break;
	}

	// If movement is finished, set next direction and halt for a time
	if( m_fCurrentInterpolation > 1.f && m_eCurrentStatus != EMovingStatus::EMovingStatus_Halted )
	{
		if( m_eCurrentStatus == EMovingStatus::EMovingStatus_Forwards )
		{
			m_eNextState = EMovingStatus::EMovingStatus_Backwards;
		}
		else
		{
			m_eNextState = EMovingStatus::EMovingStatus_Forwards;
		}

		m_eCurrentStatus = EMovingStatus::EMovingStatus_Halted;

		FTimerHandle sTimer;
		GetWorld()->GetTimerManager().SetTimer( sTimer, this, &AMovingPlatform::CBOnHaltTimeEnded, m_fHaltTime, false );
	}
}

//////////////////////////////////////////////////////////////////////////
// A callback function for when the halt timer finishes
//////////////////////////////////////////////////////////////////////////
void AMovingPlatform::CBOnHaltTimeEnded()
{
	// Reset movement data
	m_fCurrentInterpolation = 0.f;
	m_eCurrentStatus = m_eNextState;
}