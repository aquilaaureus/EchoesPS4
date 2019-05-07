// Fill out your copyright notice in the Description page of Project Settings.

#include "DashGhostComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UDashGhostComponent::UDashGhostComponent()
	: m_fStartDelay		( 0.0f )
	, m_fSpawnDelay		( 10.0f )
	, m_fDelayCounter	( 0.0f )
	, m_iNumOfGhosts	( 0 )
	, m_iTotalGhosts	( 3 )
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	// ...
}


// Called when the game starts
void UDashGhostComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	OnComponentActivated.AddDynamic(this, &UDashGhostComponent::SpawnStart);
	m_fStartDelay /= 1000.0f;
	m_fSpawnDelay /= 1000.0f;
}


// Called every frame
void UDashGhostComponent::TickComponent(float fDeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(fDeltaTime, TickType, ThisTickFunction);

	// ...

	m_fDelayCounter += fDeltaTime;

	if (m_fDelayCounter >= (m_fStartDelay + m_fSpawnDelay))
	{
		FActorSpawnParameters psDefaultSpawnParameter;
		psDefaultSpawnParameter.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		GetWorld()->SpawnActor<AActor>(m_pcGhostBP, GetOwner()->GetTransform(), psDefaultSpawnParameter);

		--m_iNumOfGhosts;

		if (m_iNumOfGhosts <= 0)
		{
			Deactivate();
		}
		else 
		{
			m_fDelayCounter = m_fStartDelay; //because we are always checking for the full delay
		}
	}
}

void UDashGhostComponent::SpawnStart(UActorComponent * pcComponent, bool bReset)
{
	//If the ghost mesh actor is not set, no need to run the component
	if (!m_pcGhostBP)
	{
		Deactivate();
		return;
	}

	m_fDelayCounter = 0.f;
	m_iNumOfGhosts = m_iTotalGhosts;
}

