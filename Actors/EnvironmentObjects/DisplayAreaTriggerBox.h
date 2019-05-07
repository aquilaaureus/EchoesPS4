// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "DisplayAreaTriggerBox.generated.h"

//fwd declarations
class UUserWidget;

/**
 * Simple class that provides a trigger box that once collided with, displays a widget that was uproperty assigned.
 * Currently the text is set via a text member with name 'text' on the target widget
 * If boolean is set to true, it will re-display the widget every time the collision box is re-collided with.
 */
UCLASS()
class ECHOES_API ADisplayAreaTriggerBox : public ATriggerBox
{
	GENERATED_BODY()

private:

	//Function called when the collision component begins overlap with another Actor.
	UFUNCTION()
	virtual void OnOverlapBegin(	UPrimitiveComponent* pcOverlappedComp, AActor* pcOtherActor, UPrimitiveComponent* pcOtherComp, 
									int32 iOtherBodyIndex, bool bFromSweep, const FHitResult& k_sSweepResult );

	// Set text on the widget to display member
	void SetTextOnWidget();

	// Boolean to disable re-anable display on entering area.
	UPROPERTY( EditAnywhere, Category = "Display Area TB", DisplayName = "Re-activatable" )
	bool m_bIsTriggerReactivatable;
	
	// Was the trigger overlapped
	bool m_bWasOverlapped;

	// Widget to display
	UPROPERTY( EditAnywhere, Category = "Display Area TB", DisplayName = "Widget to Display" )
	TSubclassOf<UUserWidget> m_pcDisplayWidgetClass;

	// Widget instance
	UUserWidget* m_pcDisplayWidgetInstance;

	// Text to be displayed - replaces content of widget text member
	UPROPERTY( EditAnywhere, Category = "Display Area TB", DisplayName = "Text to Display" )
	FText m_pcDisplayText;

public:
	// Default constructor
	ADisplayAreaTriggerBox();

};
