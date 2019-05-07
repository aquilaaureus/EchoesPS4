// Fill out your copyright notice in the Description page of Project Settings.

// Header Include
#include "CheckpointInteract.h"

// Other includes
#include "Actors/Kira/EchoesCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "PlayerComponents/PlayerStatsComponent.h"
#include "Sound/SoundCue.h"
#include "WidgetBlueprintLibrary.h"

//////////////////////////////////////////////////////////////////////////
// Sets default values
//////////////////////////////////////////////////////////////////////////
ACheckpointInteract::ACheckpointInteract()
: m_pcLoadingWidgetInstance ( nullptr )
, m_bRecentlyActivated		( false )
{
	// Create the components needed for this fountain
	m_pcBreakableMesh = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "BreakableMesh" ) );
	m_pcBreakableMesh->AttachToComponent( RootComponent, FAttachmentTransformRules::KeepRelativeTransform );

	// Create the particle system component
	m_pcParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>( TEXT( "ParticleComponent" ) );
	m_pcParticleSystemComponent->AttachToComponent( RootComponent, FAttachmentTransformRules::KeepRelativeTransform );
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
void ACheckpointInteract::InstantiateLoadWidget()
{
	//Create instance of BP widget if valid, else log error
	if ( m_pcLoadingWidgetClass != nullptr )
	{
		//Only instantiate if the player controller & world is valid
		if (IsValid( GetWorld() ) && IsValid( GetWorld()->GetFirstPlayerController() ))
		{
			m_pcLoadingWidgetInstance = UWidgetBlueprintLibrary::Create( this, m_pcLoadingWidgetClass, GetWorld()->GetFirstPlayerController() );
		}
		else
		{
			UE_LOG( LogTemp, Error, TEXT( "World or PlayerController was not valid for fountain widget instantiation!" ) );
		}
	}
	else
	{
		UE_LOG( LogTemp, Error, TEXT( "No LoadingWidget Class was set on Fountain!" ) );
	}
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
void ACheckpointInteract::PlayLoadWidget()
{
	//If widget is valid, add to viewport
	if ( m_pcLoadingWidgetInstance )
	{
		m_pcLoadingWidgetInstance->AddToViewport();
	}
	else 
	{
		UE_LOG( LogTemp, Error, TEXT( "LoadingWidget instance not set!" ) );
	}
}

//////////////////////////////////////////////////////////////////////////
// Callback function for when the player interacts with the object
//////////////////////////////////////////////////////////////////////////
void ACheckpointInteract::CBInteract()
{
	// Play the Active SFX
	if ( m_pcActiveSfx )
	{
		UGameplayStatics::PlaySoundAtLocation( this, m_pcActiveSfx, GetActorLocation() );
	}

	//just in case we have a nullptr when we reach here (It should NEVER happen)
	if (!m_pcPlayerCharacter)
	{
		return;
	}

	// If its the first interaction with the fountain, activate it
	if( m_pcBreakableMesh )
	{
		if( !m_pcBreakableMesh->IsPendingKill() )
		{
			m_pcBreakableMesh->DestroyComponent();
		}
	}

	//Only play an animation if there isn't one already playing on this widget
	if ( m_pcLoadingWidgetInstance  && !m_pcLoadingWidgetInstance->IsAnyAnimationPlaying() )
	{
		PlayLoadWidget();
	}

	// Set the players respawn location and add health
	m_pcPlayerCharacter->SetRespawnLocation();
	UPlayerStatsComponent* pcPlayerStatsComp = Cast<UPlayerStatsComponent>( m_pcPlayerCharacter->GetComponentByClass( UPlayerStatsComponent::StaticClass() ) );
	if (pcPlayerStatsComp)
	{
		pcPlayerStatsComp->AddHealth( pcPlayerStatsComp->GetMaxHealth() );
	}

	if( m_pcActivatedPS && !m_bRecentlyActivated )
	{
		m_pcParticleSystemComponent->SetTemplate( m_pcActivatedPS );
		m_pcParticleSystemComponent->ActivateSystem();
		m_bRecentlyActivated = true;
	}

	// update the map
	FireMapEvent();
}

//////////////////////////////////////////////////////////////////////////
// Called when play starts or object spawned
//////////////////////////////////////////////////////////////////////////
void ACheckpointInteract::BeginPlay()
{
	Super::BeginPlay();

	InstantiateLoadWidget();

	if( m_pcPassivePS )
	{
		m_pcParticleSystemComponent->SetTemplate( m_pcPassivePS );
		m_pcParticleSystemComponent->ActivateSystem();
	}
	if ( m_pcIdleSfx )
	{
		UGameplayStatics::PlaySoundAtLocation( this, m_pcIdleSfx, GetActorLocation() );
	}
}

//////////////////////////////////////////////////////////////////////////
// Called once per frame
//////////////////////////////////////////////////////////////////////////
void ACheckpointInteract::Tick( float fDeltaTime )
{
	if( m_bRecentlyActivated && m_pcParticleSystemComponent->HasCompleted() )
	{
		if( m_pcActivePS )
		{
			m_bRecentlyActivated = false;
			m_pcParticleSystemComponent->SetTemplate( m_pcActivePS );
			m_pcParticleSystemComponent->ActivateSystem();
		}
	}
}