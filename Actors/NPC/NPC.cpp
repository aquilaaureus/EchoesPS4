// Fill out your copyright notice in the Description page of Project Settings.

// Header include
#include "NPC.h"

// Other includes
#include "Actors/Kira/EchoesCharacter.h"
#include "Components/BillboardComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Data/DialogueData.h"
#include "Data/XMLFileReader.h"
#include "Echoes/EchoesGameMode.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "NPCAnimInstance.h"
#include "TimerManager.h"
#include "UISystems/HUD/DialogueWidget.h"
#include "UISystems/UIEventDispatcher.h"

//////////////////////////////////////////////////////////////////////////
// Sets default values
//////////////////////////////////////////////////////////////////////////
ANPC::ANPC()
: m_pcDialogueWidgetInst	( nullptr )
, m_pcPlayerCharacter		( nullptr )
, m_sRemovalTimer			( FTimerHandle() )
, m_pcAnimInstance			( nullptr )
, m_iConversationPhase		( 0 )
, m_bConversationEnded		( false )
, m_pcSkeletalMesh			( nullptr )
, m_pcInteractionSphere		( nullptr )
, m_pcBillboardComponent	( nullptr )
, m_pcDialogueWidget		( nullptr )
, m_cCharacterName			( FString( "" ) )
, m_cGreetingsID			( FString( "" ) )
, m_cIntroductionID			( FString( "" ) )
, m_cNewItemsID				( FString( "" ) )
, m_cNoNewItemsID			( FString( "" ) )
, m_cFarewellsID			( FString( "" ) )
, m_fInteractionRadius		( 500.f )
, m_fRemoveTime				( 2.f )
{
	// Create the skeletal mesh component as the root
	m_pcSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>( TEXT( "SkeletalMesh" ) );
	SetRootComponent( m_pcSkeletalMesh );
	m_pcSkeletalMesh->SetCollisionObjectType( ECollisionChannel::ECC_PhysicsBody );

	// Create the interaction sphere for the player to enter
	m_pcInteractionSphere = CreateDefaultSubobject<USphereComponent>( TEXT( "InteractionSphere" ) );
	m_pcInteractionSphere->AttachToComponent( RootComponent, FAttachmentTransformRules::KeepRelativeTransform );

	// Set the collision listener to only interact when the player enters
	FCollisionResponseContainer sCollisionResponses = FCollisionResponseContainer();
	sCollisionResponses.SetAllChannels( ECollisionResponse::ECR_Ignore );
	sCollisionResponses.SetResponse( ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap );
	m_pcInteractionSphere->SetCollisionResponseToChannels( sCollisionResponses );

	// Bind the on overlap event of the interaction sphere
	m_pcInteractionSphere->OnComponentBeginOverlap.AddDynamic( this, &ANPC::CBOnOverlapBegin );
	m_pcInteractionSphere->OnComponentEndOverlap.AddDynamic( this, &ANPC::CBOnOverlapEnd );

	// Create the billboard component used to display the interact icon
	m_pcBillboardComponent = CreateDefaultSubobject<UBillboardComponent>( TEXT( "InteractBillboard" ) );
	m_pcBillboardComponent->AttachToComponent( RootComponent, FAttachmentTransformRules::KeepRelativeTransform );
	m_pcBillboardComponent->SetHiddenInGame( false );
	m_pcBillboardComponent->SetVisibility( false );
}

//////////////////////////////////////////////////////////////////////////
// Called when the game starts or when spawned
//////////////////////////////////////////////////////////////////////////
void ANPC::BeginPlay()
{
	AActor::BeginPlay();

	// If the class blueprint for the widget is set, create one to use
	if( m_pcDialogueWidget )
	{
		m_pcDialogueWidgetInst = CreateWidget<UDialogueWidget>( GetGameInstance(), m_pcDialogueWidget );

		// If the dialogue widget was created successfully
		if( !m_pcDialogueWidgetInst )
		{
			UE_LOG( LogTemp, Error, TEXT( "A widget of the provided class %s could not be created" ), *m_pcDialogueWidget->GetName() );
		}
	}
	else
	{
		UE_LOG( LogTemp, Error, TEXT( "A dialogue widget hasn't been provided" ) );
	}

	// Find the pointer to the animation instance
	if( m_pcSkeletalMesh->GetAnimInstance() )
	{
		m_pcAnimInstance = Cast<UNPCAnimInstance>( m_pcSkeletalMesh->GetAnimInstance() );

		if( !m_pcAnimInstance  )
		{
			UE_LOG( LogTemp, Error, TEXT( "The animation blueprint %s on NPC %s cannot be cast to UNPCAnimInstance" ),
				*m_pcSkeletalMesh->GetAnimInstance()->GetName(), *GetName() );
		}
	}
	else
	{
		UE_LOG( LogTemp, Error, TEXT( "No animation blueprint found on NPC: %s" ), *GetName() );
	}

	// Check if the name has been set
	if( m_cCharacterName.IsEmpty() )
	{
		UE_LOG( LogTemp, Error, TEXT( "A Name for NPC %s has not been provided" ), *GetName() );
	}

	m_pcBillboardComponent->SetRelativeLocation( FVector( 0.f, 0.f, 250.f ) );
	m_pcInteractionSphere->SetSphereRadius( m_fInteractionRadius );
}

//////////////////////////////////////////////////////////////////////////
// Delegate function to be called when player overlaps the collider
//////////////////////////////////////////////////////////////////////////
void ANPC::CBOnOverlapBegin( UPrimitiveComponent* pcHitComp, AActor* pcOtherActor, UPrimitiveComponent* pcOtherComp,
	int32 iOtherBodyIndex, bool bFromSweep, const FHitResult& rsSweepResult )
{
	m_pcPlayerCharacter = Cast<AEchoesCharacter>( pcOtherActor );

	if( m_pcPlayerCharacter )
	{
		// Fire a map event for found npc
		AEchoesGameMode* pcEchoesMode = Cast<AEchoesGameMode>( UGameplayStatics::GetGameMode( this ) );
		if( pcEchoesMode )
		{
			AUIEventDispatcher* pcEventDispatcher = pcEchoesMode->GetUIEventDispatcher();
			if( pcEventDispatcher )
			{
				pcEventDispatcher->OnMapAnnotationUpdate.Broadcast( m_cCharacterName );
			}
			else
			{
				UE_LOG( LogTemp, Warning, TEXT( "Map Event Dispatcher is nullptr on %s" ), *GetName() );
			}
		}
		else
		{
			UE_LOG( LogTemp, Warning, TEXT( "Echoes Game Mode cast failed on %s" ), *GetName() );
		}

		m_iConversationPhase = 0;

		FDialogueData sDialogue = UXMLFileReader::ReadXmlData( m_cCharacterName, m_cGreetingsID, m_cDialogueFile );
		
		// Add the dialogue box to the screen
		if( m_pcDialogueWidgetInst )
		{
			m_pcDialogueWidgetInst->SetNameText( sDialogue.m_strTextName );
			m_pcDialogueWidgetInst->SetDialogueText( sDialogue.m_astrTextMessage[ FMath::RandRange( 0, sDialogue.m_astrTextMessage.Num() - 1 ) ] );

			//if( GetWorld()->GetTimerManager().TimerExists( m_sRemovalTimer ) )
			//{
			//	GetWorld()->GetTimerManager().ClearTimer( m_sRemovalTimer );
			//}
			//else
			//{
			if( !m_pcDialogueWidgetInst->IsInViewport() )
			{
				m_pcDialogueWidgetInst->AddToViewport();
			}
			//}
		}

		// Bind the input for the interaction
		EnableInput( Cast<APlayerController>( m_pcPlayerCharacter->GetController() ) );
		InputComponent->BindAction( "Interact", IE_Pressed, this, &ANPC::CBInteract );

		// Display the interact icon
		m_pcBillboardComponent->SetVisibility( true );
	}
}

//////////////////////////////////////////////////////////////////////////
// Delegate function to be called when the player leaves the collider
//////////////////////////////////////////////////////////////////////////
void ANPC::CBOnOverlapEnd( UPrimitiveComponent* pcOverlappedComp, AActor* pcOtherActor,
	UPrimitiveComponent* pcOtherComp, int32 iOtherBodyIndex )
{
	// If the player leaves display a random farewell message for a short time
	if( Cast<AEchoesCharacter>( pcOtherActor ) )
	{
		// Change the animation variable if animation blueprint exists
		if( m_pcAnimInstance )
		{
			m_pcAnimInstance->m_bIsConversing = false;
		}

		//FDialogueData sDialogue = UNarrativeDataReader::ReadXmlData( m_cCharacterName, m_cFarewellsID, m_cDialogueFile );

		//if( m_pcDialogueWidgetInst )
		//{
		//	m_pcDialogueWidgetInst->SetDialogueText( sDialogue.m_astrTextMessage[ FMath::RandRange( 0, sDialogue.m_astrTextMessage.Num() - 1 ) ] );
		//}

		//// Create a timer callback for removing the widget from viewport
		//GetWorld()->GetTimerManager().SetTimer( m_sRemovalTimer, this, &ANPC::CBRemoveWidget, m_fRemoveTime, false );

		ResetInput();
		CBRemoveWidget();
	}
}

//////////////////////////////////////////////////////////////////////////
// Callback function for interacting with the npc
//////////////////////////////////////////////////////////////////////////
void ANPC::CBInteract()
{
	// Add action for closing the dialogue popup
	InputComponent->BindAction( "ClosePopup", IE_Pressed, this, &ANPC::CBInteract );

	// Change the animation variable if animation blueprint exists
	if( m_pcAnimInstance )
	{
		m_pcAnimInstance->m_bIsConversing = true;
	}

	if( !m_bConversationEnded )
	{
		// If the conversation is not over get the next line of dialogue
		FDialogueData sDialogue = UXMLFileReader::ReadXmlData( m_cCharacterName, m_cIntroductionID, m_cDialogueFile );

		if( m_pcDialogueWidgetInst )
		{
			if( m_iConversationPhase >= sDialogue.m_astrTextMessage.Num() )
			{
				CBRemoveWidget();
				m_bConversationEnded = true;
				m_iConversationPhase = 0;
			}
			else
			{
				m_pcDialogueWidgetInst->SetDialogueText(sDialogue.m_astrTextMessage[m_iConversationPhase++]);
			}
		}
	}
	else
	{
		//// If the conversation is ended display a goodbye message
		//FDialogueData sDialogue = UNarrativeDataReader::ReadXmlData( m_cCharacterName, m_cNoNewItemsID, m_cDialogueFile );

		//if( m_pcDialogueWidgetInst )
		//{
		//	m_pcDialogueWidgetInst->SetDialogueText( sDialogue.m_astrTextMessage[ m_iConversationPhase ] );

		//	++m_iConversationPhase;
		//	if( m_iConversationPhase >= sDialogue.m_astrTextMessage.Num() )
		//	{
		//		m_iConversationPhase = 0;
		//	}
		//}

	}
}

//////////////////////////////////////////////////////////////////////////
// Callback function for removing the widget from the viewport when the player leaves
//////////////////////////////////////////////////////////////////////////
void ANPC::CBRemoveWidget()
{
	if( m_pcDialogueWidgetInst )
	{
		if( m_pcDialogueWidgetInst->IsInViewport() )
		{
			m_pcDialogueWidgetInst->RemoveFromViewport();
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Resets the input once the player leaves the NPC
//////////////////////////////////////////////////////////////////////////
void ANPC::ResetInput()
{
	// Remove the input bindings created for this
	InputComponent->ClearActionBindings();
	if (m_pcPlayerCharacter && m_pcPlayerCharacter->GetController())
	{
		DisableInput(Cast<APlayerController>(m_pcPlayerCharacter->GetController()));
	}
	m_pcBillboardComponent->SetVisibility( false );
}

