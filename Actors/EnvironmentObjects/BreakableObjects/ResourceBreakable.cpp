// Fill out your copyright notice in the Description page of Project Settings.

// Header include
#include "ResourceBreakable.h"

// Other includes
#include "Actors/Pickups/ResourcePickup.h"
#include "Engine/World.h"
#include "Echoes/Actors/Kira/EchoesCharacter.h"
#include <Particles/ParticleSystem.h>
#include "Kismet/GameplayStatics.h"
#include "Runtime/Core/Public/Math/UnrealMathUtility.h"
#include "Components/StaticMeshComponent.h"


//////////////////////////////////////////////////////////////////////////
// Sets default values
//////////////////////////////////////////////////////////////////////////
AResourceBreakable::AResourceBreakable()
	: m_pcResourcePickup	( nullptr )
	, m_bDropsShard			( false )
	, m_iHealth				( 3 )
	, m_fMaxDeflectionAngle	( 2.0f )
	, m_fShakeMs			( 0.5f )
	, m_fStubsScale			( 0.75f )
{ 
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

//////////////////////////////////////////////////////////////////////////
// Called when play starts or actor spawned
//////////////////////////////////////////////////////////////////////////
void AResourceBreakable::BeginPlay()
{
	Super::BeginPlay();

	if( !m_pcResourcePickup )
	{
		UE_LOG( LogTemp, Error, TEXT( "No resource class to be spawned found on %s" ), *GetName() );
	}

	m_bShaking = false;
	m_fShakeTime = 0.0f;
	m_sTransform = GetActorTransform();
}

//////////////////////////////////////////////////////////////////////////
// A function UE4 has to control the damage the ice will take from the character
//////////////////////////////////////////////////////////////////////////
float AResourceBreakable::TakeDamage( float fDamageAmount, FDamageEvent const& krsDamageEvent,
	AController* pcEventInstigator, AActor* pcDamageCauser )
{
	AEchoesCharacter* pcPlayerCharacter = Cast<AEchoesCharacter> ( pcDamageCauser );

	//If Dead or if it was hit by a charged attack
	if (--m_iHealth <= 0 || (pcPlayerCharacter && pcPlayerCharacter->IsLastAttackWasChargedAttack()) )
	{
		// Drop a pickup if necessary
		if (m_bDropsShard && m_pcResourcePickup)
		{
			GetWorld()->SpawnActor<AActor>( m_pcResourcePickup, GetActorLocation() + FVector( 0.f, 0.f, 200.f ), GetActorRotation() );
		}

		// Call base class function for base breakable functionality: This calls destroy and plays the destroy sound
		ABreakableObject::TakeDamage(fDamageAmount, krsDamageEvent, pcEventInstigator, pcDamageCauser);

		if (m_pcStubsActor)
		{
			m_sTransform.SetScale3D(m_sTransform.GetScale3D()*m_fStubsScale);
			GetWorld()->SpawnActor<AActor>(m_pcStubsActor, m_sTransform);
		}
	}
	else
	{
		if (m_pcDamageSound)
		{
			UGameplayStatics::PlaySoundAtLocation((UObject*)this, (USoundBase*)m_pcDamageSound, GetActorLocation());
		}

		if ( m_apcHealthMeshes.IsValidIndex(m_iHealth-1) )
		{ 
			m_pcStaticMeshComponent->SetStaticMesh(m_apcHealthMeshes.GetData()[m_iHealth-1]);
		}

		//decrease scale. Use this to (also?) change textures. Also possibly use a secondary Damage SFX?
		//FVector vNewScale = (m_pcStaticMeshComponent->GetRelativeTransform().GetScale3D())*(1-(m_iHealth/16.0f));
		//m_pcStaticMeshComponent->SetRelativeScale3D(vNewScale);
		m_bShaking = true;
		PrimaryActorTick.SetTickFunctionEnable( true );
	}

	if (m_pcDamagePatricleEmitter)
	{
		//Spawn the damage VFX (Every time we strike)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), m_pcDamagePatricleEmitter, GetActorLocation(), FRotator::ZeroRotator, true);
	}

	return fDamageAmount;
}

void AResourceBreakable::Tick(float fTimeDelta)
{
	Super::Tick(fTimeDelta);

	if (m_bShaking)
	{
		m_fShakeTime += fTimeDelta;
		if (m_fShakeTime <= m_fShakeMs)
		{
			m_v3fRotation.X = FMath::RandRange(-m_fMaxDeflectionAngle, m_fMaxDeflectionAngle) + m_sTransform.GetRotation().Euler().X;
			m_v3fRotation.Y = FMath::RandRange(-m_fMaxDeflectionAngle, m_fMaxDeflectionAngle) + m_sTransform.GetRotation().Euler().Y;
			m_v3fRotation.Z = FMath::RandRange(-m_fMaxDeflectionAngle, m_fMaxDeflectionAngle) + m_sTransform.GetRotation().Euler().Z;
			m_pcStaticMeshComponent->SetRelativeRotation(FRotator::MakeFromEuler(m_v3fRotation));
		}
		else
		{
			m_pcStaticMeshComponent->SetRelativeRotation(m_sTransform.GetRotation());
			m_fShakeTime = 0.0f;
			m_bShaking = false;
			PrimaryActorTick.SetTickFunctionEnable( false );
		}
	}
}
