// Fill out your copyright notice in the Description page of Project Settings.

// Header include
#include "EnvironmentObject.h"

// Other includes
#include "Components/StaticMeshComponent.h"

//////////////////////////////////////////////////////////////////////////
// Sets default values
//////////////////////////////////////////////////////////////////////////
AEnvironmentObject::AEnvironmentObject()
{
	m_pcStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "StaticMeshComponent" ) );
	SetRootComponent( m_pcStaticMeshComponent );
}

