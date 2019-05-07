// Fill out your copyright notice in the Description page of Project Settings.

// Header Include
#include "AbilityInteract.h"

// Other Includes
#include "Actors/Kira/EchoesCharacter.h"
#include "Animation/ChestAnimInstance.h"
#include "Components/BillboardComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EchoesGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundCue.h"
#include "UISystems/UIEventDispatcher.h"
#include "UISystems/Widgets/MenuWidgetPlayerController.h"
#include "WidgetBlueprintLibrary.h"

//////////////////////////////////////////////////////////////////////////
// Sets the default values
//////////////////////////////////////////////////////////////////////////
AAbilityInteract::AAbilityInteract()
: fAngleToLookAt					( 125.f )
, m_pcEchoesCharacter				( nullptr )
, m_pcChestAnimation				( nullptr )
, m_pcAbilityWidgetInstance			( nullptr )
, m_pcPlayerController				( nullptr )
, m_sAbilityInteractForwardVector	( FVector::ZeroVector )
, m_pcSkeletalMeshComponent			( nullptr )
, m_pcBoxColliderComponent			( nullptr )
, m_pcIdleSfx						( nullptr )
, m_pcActivationSfx					( nullptr )
, m_pcAbilityWidget					( nullptr )
, m_eAbilityPickup					( EAbility::EAbility_None )
{ 
	m_pcSkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>( TEXT( "SkeletalMEsh" ) );
	SetRootComponent( m_pcSkeletalMeshComponent );

	m_pcBoxColliderComponent = CreateDefaultSubobject<UBoxComponent>( TEXT( "Box Collider Component" ) );

	// Reattach sub components and destroy static mesh component
	// Run if checks to ensure that they exist - Only one that may not exist is m_pcBoxColliderComponent
	// Still better to run safely
	if ( m_pcBoxColliderComponent )
	{
		m_pcBoxColliderComponent->AttachToComponent	( RootComponent, FAttachmentTransformRules::KeepRelativeTransform );
	}

	if ( m_pcInteractionSphere )
	{
		m_pcInteractionSphere->AttachToComponent ( RootComponent, FAttachmentTransformRules::KeepRelativeTransform );
	}

	if ( m_pcBillboardComponent )
	{
		m_pcBillboardComponent->AttachToComponent	( RootComponent, FAttachmentTransformRules::KeepRelativeTransform );
	}

	m_pcStaticMeshComponent->DestroyComponent();
	m_pcStaticMeshComponent = nullptr;
}

//////////////////////////////////////////////////////////////////////////

void AAbilityInteract::Tick( float fInDeltaTime )
{
	// If the player is in the Box Collider
	if ( m_bIsPlayerOverlapping )
	{
		if ( m_pcEchoesCharacter )
		{
			//if ( pcEchoesCharacter->m_bIsOpeningChest )
			//////////////////////////////////////////////////////////////////////////
			// Every Tick is taking a MINIMUM of 16 bytes in memory allocation.
			// MAXIMUM of 30 bytes will be assigned in this tick alone (excluding external funcs)
			//////////////////////////////////////////////////////////////////////////

			// Get the Forward Vector of the player at this frame
			// 12 bytes
			FVector sPlayerForwardVector = m_pcEchoesCharacter->GetActorForwardVector();

			// Obtain the Dot Product of these two Forward Vectors
			// 4 bytes
			float fAbilityDotProd = UKismetMathLibrary::Dot_VectorVector( sPlayerForwardVector, m_sAbilityInteractForwardVector );

			// Obtain the inverse Cos of the Dot Product
			m_fInvCos = UKismetMathLibrary::DegAcos( fAbilityDotProd );

			// If the Box Collider Component exists
			if ( m_pcBoxColliderComponent )
			{
				if ( m_pcBoxColliderComponent->IsAttachedTo( RootComponent ) )
				{
					// Check if the player is actually looking in the "general" direction of the Interact Mesh 
					// And if the Input is not current Active
					if ( m_fInvCos >= fAngleToLookAt )
					{
						// If the Input has not already been enabled
						if ( !m_bInputEnabled )
						{
							// Make the Interaction Billboard Prompt Invisible 
							m_pcBillboardComponent->SetVisibility( true );

							// Allow Input to be enabled
							m_bInputEnabled = true;

							// The Dynamics will need to be removed if this condition isn't met
							m_bNeedToRemoveDynamics = true;

							// Enable the interaction with the Ability Pickup
							if ( m_bInputEnabled )
							{
								EnableInteract();
							}
						}
					}
					else
					{
						RemoveInteraction();
					}
				}
			}
		}
	}
	else
	{
		//ResetInput();
	}

	// If the animation is finished display the ability popups
	if( m_pcChestAnimation && !m_pcAbilityWidgetInstance )
	{
		if( m_pcChestAnimation->m_bOpeningComplete )
		{
			m_pcPlayerCharacter->SetOpeningChestAnimState( false );

			if( m_pcAbilityWidget )
			{
				// Create and display the ability popup
				m_pcAbilityWidgetInstance = UWidgetBlueprintLibrary::Create( this, m_pcAbilityWidget, GetWorld()->GetFirstPlayerController() );
				m_pcAbilityWidgetInstance->AddToPlayerScreen();

				// Bind the input for the interaction
				EnableInput( Cast<APlayerController>( m_pcPlayerCharacter->GetController() ) );
				InputComponent->BindAction( "ClosePopup", IE_Pressed, this, &AAbilityInteract::CBClosePopup ).bExecuteWhenPaused = true;

				// Set the Input Mode to UI Only
				FInputModeGameAndUI sInputModeData;

				// Get local reference to the player controller
				if( nullptr == m_pcPlayerController )
				{
					m_pcPlayerController = Cast<AMenuWidgetPlayerController>( GetWorld()->GetFirstPlayerController() );
				}

				if( m_pcPlayerController )
				{
					// Set the Input Mode
					m_pcPlayerController->SetInputMode( sInputModeData );

					// Only stop the input on the pawn
					m_pcPlayerController->GetPawn()->DisableInput( m_pcPlayerController );

					// Set the focus of the player to this widget
					m_pcAbilityWidgetInstance->bIsFocusable = true;
					m_pcAbilityWidgetInstance->SetKeyboardFocus();
					m_pcAbilityWidgetInstance->SetUserFocus( GetWorld()->GetFirstPlayerController() );
				}

				if (m_pcAbilityPopupSound)
				{
					UGameplayStatics::PlaySound2D ( GetWorld ( ) , m_pcAbilityPopupSound );
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Removes Dynamic Calls if they are ste
// Makes the Billboard Component invisible if it is visible
// Disables Input that interacts with the Ability Mesh 
//////////////////////////////////////////////////////////////////////////
void AAbilityInteract::RemoveInteraction()
{
	m_fInvCos = 0.f;

	// If the Dynamics were previously assigned
	if ( m_bNeedToRemoveDynamics )
	{
		// Remove the dynamics from the Box Collider Component
		//RemoveBoxDynamics( m_pcBoxColliderComponent );

		// Do not remove the Dynamics again as already removed 
		m_bNeedToRemoveDynamics = false;
	}

	// Make the Interaction Billboard Prompt Invisible 
	if ( m_pcBillboardComponent->IsVisible() )
	{
		m_pcBillboardComponent->SetVisibility( false );
	}

	// Disable Input Interaction 
	if ( m_bInputEnabled )
	{
		m_bInputEnabled = false;
	}

	// Reset the input to not be able to interact with the Ability Pickup
	ResetInput();
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Called when object spawns or play starts
//////////////////////////////////////////////////////////////////////////
void AAbilityInteract::BeginPlay()
{
	AInteractableObject::BeginPlay();

	// If the player character does not exist, cast it to the first player in the world 
	if ( !m_pcEchoesCharacter )
	{
		m_pcEchoesCharacter = Cast<AEchoesCharacter>( UGameplayStatics::GetPlayerCharacter ( GetWorld (), 0 ) );
	}

	if( m_pcSkeletalMeshComponent )
	{
		if( m_pcSkeletalMeshComponent->SkeletalMesh )
		{
			m_sAbilityInteractForwardVector = GetActorForwardVector();
			
			// Set the radius of the Collision Box according to the static mesh size
			FVector sMeshSize = m_pcSkeletalMeshComponent->SkeletalMesh->GetImportedBounds().BoxExtent;

			// Set the Box Extent to be accessible only when DIRECTLY in front of the object
			// Set the position of the Box Collider to be in front of the Chest ( i.e. to open it correctly )
			if ( m_pcBoxColliderComponent )
			{
				m_pcBoxColliderComponent->SetBoxExtent( FVector( ( sMeshSize.X / 2.f ), ( sMeshSize.Y / 2.f ), ( ( sMeshSize.Z / 5.f ) + 10.f ) ), true );
				m_pcBoxColliderComponent->SetRelativeLocation( FVector( sMeshSize.X, 0.f, 0.f ), false, nullptr, ETeleportType::None );

				// Setup the Dynamic callbacks for the Box Collider
				SetBoxDynamics( m_pcBoxColliderComponent );
			}

			// Set the billboard location to be above the interaction object
			m_pcBillboardComponent->SetRelativeLocation( FVector( 0.f, 0.f, sMeshSize.Z + m_fZAddition ) );
		}
	}

	// Check editor values
	if( m_pcIdleSfx )
	{
		UGameplayStatics::PlaySoundAtLocation( this, m_pcIdleSfx, GetActorLocation() );
	}
	else
	{
		UE_LOG( LogTemp, Warning, TEXT( "Idle SFX not set on ability chest %s" ), *GetName() );
	}

	if( !m_pcActivationSfx ) { UE_LOG( LogTemp, Warning, TEXT( "Activation SFX not set on ability chest %s" ), *GetName() ); };
	if( !m_pcAbilityWidget ) { UE_LOG( LogTemp, Warning, TEXT( "No ability gained popup set on ability chest %s" ), *GetName() ); };

	// Get the animation blueprint controlling the animations and store it
	if( m_pcSkeletalMeshComponent->GetAnimInstance() )
	{
		m_pcChestAnimation = Cast<UChestAnimInstance>( m_pcSkeletalMeshComponent->GetAnimInstance() );

		if( !m_pcChestAnimation )
		{
			UE_LOG( LogTemp, Warning, TEXT( "%s could not be case to UChestAnimInstance on %s" ),
				*m_pcSkeletalMeshComponent->GetAnimInstance()->GetName(), *GetName() );
		}
	}
	else
	{
		UE_LOG( LogTemp, Warning, TEXT( "No animation instance found on %s" ), *GetName() );
	}
}

//////////////////////////////////////////////////////////////////////////
// Callback function for when the player interacts with the object
//////////////////////////////////////////////////////////////////////////
void AAbilityInteract::CBInteract() 
{
	//just in case we have a nullptr when we reach here (It should NEVER happen)
	if (!m_pcPlayerCharacter)
	{
		return;
	}

	if ( m_pcPlayerCharacter && m_bIsPlayerOverlapping )
	{
		switch ( m_eAbilityPickup )
		{
		case EAbility::EAbility_None:
			break;
		case EAbility::EAbility_DoubleJump:
			m_pcPlayerCharacter->SetDoubleJump( true );
			break;
		case EAbility::EAbility_Dash:
			m_pcPlayerCharacter->SetDashEnabled();
			break;
		case EAbility::EAbility_ChargedAttack:
			m_pcPlayerCharacter->SetChargedAttackEnabled();
			break;
		default:
			break;
		}

		if ( m_pcChestAnimation )
		{
			m_pcChestAnimation->m_bIsOpen = true;
			m_pcPlayerCharacter->SetOpeningChestAnimState( true );

			// Disable Input to the Player
			SetPlayerInput( false );
		}
	}

	// Call to remove the dynamics attached to the Box Component
	//AInteractableObject::RemoveBoxDynamics( m_pcBoxColliderComponent );

	// Tell the parent class that this interactable has a Box Collider and not a Sphere Collider
	m_bHasBoxCollider = true;

	// Call base class interact method for reset
	AInteractableObject::CBInteract();

	// Destroy the Box Collider Component
	// Ensure that the Component is a nullptr - Will break if not
	if ( m_pcBoxColliderComponent )
	{
		m_pcBoxColliderComponent->DestroyComponent( false );
		m_pcBoxColliderComponent = nullptr;
	}

	m_bIsPlayerOverlapping = false;

	// Fire the ability gained event
	AEchoesGameMode* pcEchoesGameMode = Cast<AEchoesGameMode>( UGameplayStatics::GetGameMode( this ) );
	if( pcEchoesGameMode )
	{
		AUIEventDispatcher* pcUIEventDispatcher = pcEchoesGameMode->GetUIEventDispatcher();
		if( pcUIEventDispatcher )
		{
			pcUIEventDispatcher->OnAbilityGained.Broadcast( m_eAbilityPickup );
		}
	}

	if (m_dChestOpenedDelegate.IsBound())
	{
		m_dChestOpenedDelegate.Broadcast();
	}
}

//////////////////////////////////////////////////////////////////////////
// Callback function to close the popup that has been opened
//////////////////////////////////////////////////////////////////////////
void AAbilityInteract::CBClosePopup()
{
	// Remove the widget from the parent it is attached to
	if( m_pcAbilityWidgetInstance->IsInViewport() )
	{
		m_pcAbilityWidgetInstance->RemoveFromViewport();
	}

	// Set the Input Mode to Game Only
	FInputModeGameOnly sInputModeData;

	if( m_pcPlayerController )
	{
		// Return the input to normal
		m_pcPlayerController->SetInputMode( sInputModeData );
		m_pcPlayerController->GetPawn()->EnableInput( m_pcPlayerController );

		// Remove the input bindings created for this
		InputComponent->ClearActionBindings();
		DisableInput( m_pcPlayerController );
	} 
}