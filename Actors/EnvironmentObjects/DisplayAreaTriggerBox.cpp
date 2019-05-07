// Fill out your copyright notice in the Description page of Project Settings.

//Header
#include "DisplayAreaTriggerBox.h"

//Other includes
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ShapeComponent.h"
#include "TextBlock.h"
#include "Actors/Kira/EchoesCharacter.h"
#include "WidgetBlueprintLibrary.h"


ADisplayAreaTriggerBox::ADisplayAreaTriggerBox()
: m_bIsTriggerReactivatable		(false)
, m_bWasOverlapped				(false)
, m_pcDisplayWidgetInstance		(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	//Add collision callback events
	GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic( this, &ADisplayAreaTriggerBox::OnOverlapBegin );

	//Set collision to only respond to overlapping pawn and ignore all other channels.
	FCollisionResponseContainer sCollisionResponses = FCollisionResponseContainer();
	sCollisionResponses.SetAllChannels( ECollisionResponse::ECR_Ignore );
	sCollisionResponses.SetResponse( ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap );
	GetCollisionComponent()->SetCollisionResponseToChannels( sCollisionResponses );
}

void ADisplayAreaTriggerBox::SetTextOnWidget()
{
	if (m_pcDisplayWidgetInstance)
	{
		//All recursive children
		TArray<UWidget*> tapcChildWidgets;
		m_pcDisplayWidgetInstance->WidgetTree->GetChildWidgets( m_pcDisplayWidgetInstance->GetRootWidget(), tapcChildWidgets );

		FString pcCurrentElementName;
		for (UWidget* pcWidget : tapcChildWidgets)
		{
			pcCurrentElementName = pcWidget->GetName();

			//Set widget switcher ref
			if (pcCurrentElementName.Compare( TEXT( "Text" ) ) == 0)
			{
				UTextBlock* pcTextBlock = Cast<UTextBlock>( pcWidget );
				if ( pcTextBlock )
				{
					pcTextBlock->SetText( m_pcDisplayText );
				}
			}
		}
	}
}

void ADisplayAreaTriggerBox::OnOverlapBegin( UPrimitiveComponent* pcOverlappedComp, AActor* pcOtherActor, UPrimitiveComponent* pcOtherComp, int32 iOtherBodyIndex, bool bFromSweep, const FHitResult& k_sSweepResult )
{
	//Only activate if first run or reactivatable.
	if ( !m_bWasOverlapped || m_bIsTriggerReactivatable )
	{
		AEchoesCharacter* pcPlayer = Cast<AEchoesCharacter>( pcOtherActor );
		if (IsValid( pcPlayer ) && !m_pcDisplayWidgetInstance)
		{
			//If class is valid, instantiate the widget
			if (m_pcDisplayWidgetClass != nullptr)
			{
				m_pcDisplayWidgetInstance = UWidgetBlueprintLibrary::Create( this, m_pcDisplayWidgetClass, GetWorld()->GetFirstPlayerController() );
			}
			else
			{
				UE_LOG( LogTemp, Error, TEXT( "No DisplayWidget Class was set on DisplayArea-TB!" ) );
			}
		}

		//If widget is valid, add to viewport provided not currently playing
		if (m_pcDisplayWidgetInstance && !m_pcDisplayWidgetInstance->IsAnyAnimationPlaying())
		{
			SetTextOnWidget();
			m_pcDisplayWidgetInstance->AddToViewport();
			m_bWasOverlapped = true;

			// If this trigger cannot be re-activated, destroy self.
			if (!m_bIsTriggerReactivatable)
			{
				SetLifeSpan( 10.0f );
			}
		}
	}
}