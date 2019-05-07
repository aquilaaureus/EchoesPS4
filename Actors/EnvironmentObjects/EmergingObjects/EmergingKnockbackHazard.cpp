// Fill out your copyright notice in the Description page of Project Settings.

// Header include
#include "EmergingKnockbackHazard.h"

// Other includes
#include "PhysicsEngine/RadialForceComponent.h"

//////////////////////////////////////////////////////////////////////////
// Sets default values
//////////////////////////////////////////////////////////////////////////
AEmergingKnockbackHazard::AEmergingKnockbackHazard()
: m_pcForceKnockbackUpper	( nullptr )
, m_pcForceKnockbackLower	( nullptr )
, m_fKnockbackForce			( 100000.f )
, m_fKnockbackRadius		( 100.f )
{
	// Create the force knockback component
	m_pcForceKnockbackUpper = CreateDefaultSubobject<URadialForceComponent>( TEXT( "ForceKnockbackUpper" ) );
	m_pcForceKnockbackUpper->AttachToComponent( RootComponent, FAttachmentTransformRules::KeepRelativeTransform );

	m_pcForceKnockbackLower = CreateDefaultSubobject<URadialForceComponent>( TEXT( "ForceKnockbackLower" ) );
	m_pcForceKnockbackLower->AttachToComponent( RootComponent, FAttachmentTransformRules::KeepRelativeTransform );
}

//////////////////////////////////////////////////////////////////////////
// Called when the game starts or when spawned
//////////////////////////////////////////////////////////////////////////
void AEmergingKnockbackHazard::BeginPlay()
{
	Super::BeginPlay();

	// Calculate/Set values from the editor
	m_pcForceKnockbackUpper->ImpulseStrength = m_fKnockbackForce;
	m_pcForceKnockbackLower->ImpulseStrength = m_fKnockbackForce;

	m_pcForceKnockbackUpper->Radius = m_fKnockbackRadius;
	m_pcForceKnockbackLower->Radius = m_fKnockbackRadius;
}

//////////////////////////////////////////////////////////////////////////
// Function to be overwritten in children to control behaviour for after the hazard has emerged
//////////////////////////////////////////////////////////////////////////
void AEmergingKnockbackHazard::EmergeCompleted()
{
	m_pcForceKnockbackUpper->FireImpulse();
	m_pcForceKnockbackLower->FireImpulse();
}