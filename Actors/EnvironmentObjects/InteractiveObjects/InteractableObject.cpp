// Fill out your copyright notice in the Description page of Project Settings.

// Header include
#include "InteractableObject.h"

// Other includes
#include "Actors/Kira/EchoesCharacter.h"
#include "Components/BillboardComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EchoesGameMode.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "UISystems/UIEventDispatcher.h"
#include "UISystems/Widgets/MenuWidgetPlayerController.h"
#include "UserWidget.h"
#include "WidgetBlueprintLibrary.h"
#include "WidgetTree.h"

//////////////////////////////////////////////////////////////////////////
// Sets default values
//////////////////////////////////////////////////////////////////////////
AInteractableObject::AInteractableObject()
: m_pcPlayerCharacter			( nullptr )
, m_fInvCos						( 0.f )
, m_bMultipleActivation			( false )
, m_bHasBoxCollider				( false )
, m_bIsPlayerOverlapping		( false )
, m_bIsPlayerFacing				( false )
, m_bInputEnabled				( false )
, m_bNeedToRemoveDynamics		( false )
, m_fRadiusIncrease				( 150.f )
, m_fZAddition					( 200.f )
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Set this to true to set the object up for the post processing stage used to create outlines and overlays.
	m_pcStaticMeshComponent->SetRenderCustomDepth( true );

	//Set value equal to 2 to activate the outline.
	//set a value HIGHER than 2 to deactivate it. We want to start with the overlay inactive.
	//Set this value to 1 or 0 to activate the secondary overlay used on the player and enemies to be visible when occluded.
	m_pcStaticMeshComponent->SetCustomDepthStencilValue( 3 );

	// Create the interaction sphere for the player to enter
	m_pcInteractionSphere = CreateDefaultSubobject<USphereComponent>( TEXT( "InteractionSphere" ) );
	m_pcInteractionSphere->AttachToComponent( RootComponent, FAttachmentTransformRules::KeepRelativeTransform );

	// Set the collision listener to only interact when the player enters
	FCollisionResponseContainer sCollisionResponses = FCollisionResponseContainer();
	sCollisionResponses.SetAllChannels( ECollisionResponse::ECR_Ignore );
	sCollisionResponses.SetResponse( ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap );
	m_pcInteractionSphere->SetCollisionResponseToChannels( sCollisionResponses );

	// Bind the on overlap event of the interaction sphere
	m_pcInteractionSphere->OnComponentBeginOverlap.AddDynamic( this, &AInteractableObject::CBOnOverlapBegin );
	m_pcInteractionSphere->OnComponentEndOverlap.AddDynamic( this, &AInteractableObject::CBOnOverlapEnd );

	// Create the billboard component used to display the interact icon
	m_pcBillboardComponent = CreateDefaultSubobject<UBillboardComponent>( TEXT( "InteractBillboard" ) );
	m_pcBillboardComponent->AttachToComponent( RootComponent, FAttachmentTransformRules::KeepRelativeTransform );
	m_pcBillboardComponent->SetHiddenInGame( false );
	m_pcBillboardComponent->SetVisibility( false );
}

//////////////////////////////////////////////////////////////////////////
// Called when the game starts or when spawned
//////////////////////////////////////////////////////////////////////////
void AInteractableObject::BeginPlay()
{
	Super::BeginPlay();

	if ( m_pcStaticMeshComponent )
	{
		if ( m_pcStaticMeshComponent->GetStaticMesh() )
		{
			// Set the radius of the collision sphere according to the static mesh size
			FVector sMeshSize = m_pcStaticMeshComponent->GetStaticMesh()->GetBoundingBox().GetSize();

			// Set the radius to the larger value of X and Y
			if ( sMeshSize.X > sMeshSize.Y )
			{
				m_pcInteractionSphere->SetSphereRadius( sMeshSize.X + m_fRadiusIncrease );
			}
			else
			{
				m_pcInteractionSphere->SetSphereRadius( sMeshSize.Y + m_fRadiusIncrease );
			}

			// Set the billboard location to be above the interaction object
			m_pcBillboardComponent->SetRelativeLocation( FVector( 0.f, 0.f, sMeshSize.Z + m_fZAddition ) );
		}
	}

	SetupEventDispatcherMember();
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Call to fire the event to update the map
//////////////////////////////////////////////////////////////////////////
void AInteractableObject::FireMapEvent()
{
	if( !m_pcUIEventDispatcher )
	{
		SetupEventDispatcherMember();
	}

	m_pcUIEventDispatcher->OnMapAnnotationUpdate.Broadcast( m_strAnnotationID );
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Enables Input on the Player and Binds the Interact functionality to them
//////////////////////////////////////////////////////////////////////////
void AInteractableObject::EnableInteract()
{
	// Bind the input for the interaction
	if ( m_pcPlayerCharacter )
	{
		EnableInput( Cast<APlayerController>( m_pcPlayerCharacter->GetController() ) );
		InputComponent->BindAction( "Interact", IE_Pressed, this, &AInteractableObject::CBInteract );
	}
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Delegate function to be called when player overlaps the collider
//////////////////////////////////////////////////////////////////////////
void AInteractableObject::CBOnOverlapBegin( UPrimitiveComponent* pcHitComp, AActor* pcOtherActor, UPrimitiveComponent* pcOtherComp,
	int32 iOtherBodyIndex, bool bFromSweep, const FHitResult& rsSweepResult )
{
	AEchoesCharacter* pcIsItKira = Cast<AEchoesCharacter>(pcOtherActor);

	//If it overlaps with anything else, ignore
	if (!pcIsItKira)
	{
		return;
	}

	//It IS Kira
	m_pcPlayerCharacter = pcIsItKira;

	if ( m_pcPlayerCharacter )
	{
		if ( !m_bIsPlayerOverlapping )
		{
			m_bIsPlayerOverlapping = true;
		}

		// If there is no Box Collider Component 
		if ( !m_bHasBoxCollider )
		{
			EnableInteract();

			// Display the interact icon
			m_pcBillboardComponent->SetVisibility( true );
		}
		
		//Set value equal to 2 to activate the outline.
		//set a value HIGHER than 2 to deactivate it. 
		//Set this value to 1 or 0 to activate the secondary overlay used on the player and enemies to be visible when occluded.
		
		// TODO: This is causing breaks needs to be fixed (Commented by S.C)
		//m_pcStaticMeshComponent->SetCustomDepthStencilValue(2);
	}
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Callback function for when the player interacts with the object
//////////////////////////////////////////////////////////////////////////
void AInteractableObject::CBInteract()
{
	FireMapEvent();

	// If the object should only activate once, remove bindings.
	if (!m_bMultipleActivation) 
	{
		// Does the Interactable have a box collider
		if ( !m_bHasBoxCollider )
		{
			// If not, remove only the sphere colliders and reset input
			RemoveSphereDynamics();
		}

		// Always reset input 
		ResetInput();
	}
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Delegate function to be called when the player leaves the collider
//////////////////////////////////////////////////////////////////////////
void AInteractableObject::CBOnOverlapEnd( UPrimitiveComponent* pcOverlappedComp, AActor* pcOtherActor,
	UPrimitiveComponent* pcOtherComp, int32 iOtherBodyIndex )
{
	AEchoesCharacter* pcIsItKira = Cast<AEchoesCharacter>(pcOtherActor);
	
	//If the ended overlap was with anything else, ignore it
	if (!pcIsItKira)
	{
		return;
	}


	m_pcPlayerCharacter = pcIsItKira;

	// If the other actor is the player character
	if ( m_pcPlayerCharacter )
	{
		// If there is no Box Collider, perform the default functionality 
		if ( !m_bHasBoxCollider )
		{
			ResetInput();
		}

		// If the player was overlapping the collider component
		if ( m_bIsPlayerOverlapping )
		{
			// Set them to not be overlapping
			m_bIsPlayerOverlapping = false;
		}

		// Call the reset input function
		// Only reset the Input if the player has stopped overlapping
		// Currently used only with chests
		if ( m_bInputEnabled )
		{
			ResetInput();
		}
	}
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Resets the input after the player has interacted or left the area
//////////////////////////////////////////////////////////////////////////
void AInteractableObject::ResetInput()
{
	// Remove the input bindings created for this
	// Only if the Character exists
	if ( m_pcPlayerCharacter )
	{
		InputComponent->ClearActionBindings();
		DisableInput( Cast<APlayerController>( m_pcPlayerCharacter->GetController() ) );
		m_pcBillboardComponent->SetVisibility( false );

		// Always reset the value of fInvCos when the player leaves the collider
		ResetInvCos();

		m_bInputEnabled = false;
	}

	//Set value equal to 2 to activate the outline.
	//set a value HIGHER than 2 to deactivate it. 
	//Set this value to 1 or 0 to activate the secondary overlay used on the player and enemies to be visible when occluded.

	// TODO: This is causing breaks needs to be fixed (Commented by S.C)
	//m_pcStaticMeshComponent->SetCustomDepthStencilValue(3);
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Sets up the pointer to the map event dispatcher used to fire events
//////////////////////////////////////////////////////////////////////////
void AInteractableObject::SetupEventDispatcherMember()
{
	// Show icon on the map
	AEchoesGameMode* pcEchoesGameMode = Cast<AEchoesGameMode>( UGameplayStatics::GetGameMode( this ) );
	if( pcEchoesGameMode )
	{
		m_pcUIEventDispatcher = pcEchoesGameMode->GetUIEventDispatcher();
	}
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Removes Dynamic calls to the Interaction Sphere 
// Adds Dynamic calls to pcBoxCollider
//////////////////////////////////////////////////////////////////////////
UBoxComponent* AInteractableObject::SetBoxDynamics( UBoxComponent* pcBoxCollider )
{
	// Remove the component overlaps so this cannot be used again
	m_pcInteractionSphere->OnComponentBeginOverlap.RemoveDynamic( this, &AInteractableObject::CBOnOverlapBegin );
	m_pcInteractionSphere->OnComponentEndOverlap.RemoveDynamic( this, &AInteractableObject::CBOnOverlapEnd );

	// Add the Component overlaps
	pcBoxCollider->OnComponentBeginOverlap.AddDynamic( this, &AInteractableObject::CBOnOverlapBegin );
	pcBoxCollider->OnComponentEndOverlap.AddDynamic( this, &AInteractableObject::CBOnOverlapEnd );

	return pcBoxCollider;
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Removes Dynamic calls to m_pcInteractionSphere
//////////////////////////////////////////////////////////////////////////
void AInteractableObject::RemoveSphereDynamics()
{
	// Remove the component overlaps so this cannot be used again
	m_pcInteractionSphere->OnComponentBeginOverlap.RemoveDynamic( this, &AInteractableObject::CBOnOverlapBegin );
	m_pcInteractionSphere->OnComponentEndOverlap.RemoveDynamic( this, &AInteractableObject::CBOnOverlapEnd );

	// Reset the input back to normal
	ResetInput();
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Removes Dynamic calls to pcBoxCollider
//////////////////////////////////////////////////////////////////////////
UBoxComponent* AInteractableObject::RemoveBoxDynamics( UBoxComponent* pcBoxCollider )
{
	// Remove the component overlaps so this cannot be used again
	pcBoxCollider->OnComponentBeginOverlap.RemoveDynamic( this, &AInteractableObject::CBOnOverlapBegin );
	pcBoxCollider->OnComponentEndOverlap.RemoveDynamic( this, &AInteractableObject::CBOnOverlapEnd );

	ResetInput();

	return pcBoxCollider;
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Set the state of Input on the Player
//////////////////////////////////////////////////////////////////////////
void AInteractableObject::SetPlayerInput( bool bInputState )
{
	// Local pointer cast to Player Controller 
	AMenuWidgetPlayerController* pcPlayerController = Cast<AMenuWidgetPlayerController>( GetWorld()->GetFirstPlayerController() );

	// If this Player Controller Exists
	if ( pcPlayerController )
	{
		// If bInputState is true
		if ( bInputState )
		{
			// Enable Input on the Player 
			pcPlayerController->GetPawn()->EnableInput( pcPlayerController );
		}
		else
		{
			// Disable Input on the Player
			pcPlayerController->GetPawn()->DisableInput( pcPlayerController );
		}
	}
}
