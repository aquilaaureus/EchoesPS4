// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Includes
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NPC.generated.h"

// Fwd dcl.
class AEchoesCharacter;
class UBillboardComponent;
class UDialogueWidget;
class UNPCAnimInstance;
class USkeletalMeshComponent;
class USphereComponent;
class UTextBlock;

//////////////////////////////////////////////////////////////////////////
// Class ANPC: A Non player character that is designed to be interactive with the 
// player when they get close. It has a UI widget to display dialogue to the player
// and uses the data reader to read this data from the xml file. It has standard
// character features such as a skeletal mesh.
//////////////////////////////////////////////////////////////////////////
UCLASS()
class ECHOES_API ANPC : public AActor
{
	GENERATED_BODY()
private:
	// The created instances of the dialogue widget
	UPROPERTY()
	UDialogueWidget* m_pcDialogueWidgetInst;
	
	// A pointer to the player character conversing with the NPC
	UPROPERTY()
	AEchoesCharacter* m_pcPlayerCharacter;

	// The timer used to remove the widget from the viewport
	UPROPERTY()
	FTimerHandle m_sRemovalTimer;

	// A point to the animation instance used on this NPC
	UPROPERTY()
	UNPCAnimInstance* m_pcAnimInstance;

	// The file that is used to read the dialogue data
	const FString m_cDialogueFile = FString( "NarrativeData.xml" );

	//////////////////////////////////////////////////////////////////////////
	// TODO: Refactor into new system 

	// The current piece of the conversation being displayed
	int m_iConversationPhase;

	// Controls whether the conversation has been ended
	bool m_bConversationEnded;

	//////////////////////////////////////////////////////////////////////////

	// Resets the input once the player leaves the NPC
	void ResetInput();

protected:
	// The skeletal mesh component used for the NPC
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "SkeletalMesh" )
	USkeletalMeshComponent* m_pcSkeletalMesh;

	// The component used to detect when the player is close
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "InteractionSphere" )
	USphereComponent* m_pcInteractionSphere;

	// The billboard component used to display an interact icon
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "InteractIcon" )
	UBillboardComponent* m_pcBillboardComponent;

	// The display widget that is used to display dialogue on the screen
	UPROPERTY( EditAnywhere, Category = "Designer Values", DisplayName = "Dialogue Widget" )
	TSubclassOf<UDialogueWidget> m_pcDialogueWidget;

	// The name of this NPC
	UPROPERTY( EditDefaultsOnly, Category = "Designer Values", DisplayName = "Character Name" )
	FString m_cCharacterName;

	//////////////////////////////////////////////////////////////////////////
	// TODO: Come up with a better system for all this

	// Speech nodes to look for greetings to the player
	UPROPERTY( EditDefaultsOnly, Category = "Designer Values|Conversation", DisplayName = "Greetings ID" )
	FString m_cGreetingsID;

	// Speech nodes to look for the introduction of the NPC to the player
	UPROPERTY( EditDefaultsOnly, Category = "Designer Values|Conversation", DisplayName = "Introduction ID" )
	FString m_cIntroductionID;

	// Speech nodes to look for new items dialogue
	UPROPERTY( EditDefaultsOnly, Category = "Designer Values|Conversation", DisplayName = "New Items ID" )
	FString m_cNewItemsID;

	// Speech nodes to look for no new items dialogue
	UPROPERTY( EditDefaultsOnly, Category = "Designer Values|Conversation", DisplayName = "No New Items ID" )
	FString m_cNoNewItemsID;

	// Speech nodes to look for farewells to the player
	UPROPERTY( EditDefaultsOnly, Category = "Designer Values|Conversation", DisplayName = "Farewells ID" )
	FString m_cFarewellsID;

	//////////////////////////////////////////////////////////////////////////

	// The radius around the NPC in which the player can interact with them
	UPROPERTY( EditDefaultsOnly, Category = "Designer Values", DisplayName = "Interaction Radius" )
	float m_fInteractionRadius;

	// The amount of time to wait after displaying a farewell message before removing the widget
	UPROPERTY( EditDefaultsOnly, Category = "Designer Values", DisplayName = "Removal Time" )
	float m_fRemoveTime;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this actor's properties
	ANPC();

	// Delegate function to be called when player overlaps the collider
	UFUNCTION()
	void CBOnOverlapBegin( UPrimitiveComponent* pcHitComp, AActor* pcOtherActor, UPrimitiveComponent* pcOtherComp,
		int32 iOtherBodyIndex, bool bFromSweep, const FHitResult& rsSweepResult );

	// Delegate function to be called when the player leaves the collider
	UFUNCTION()
	void CBOnOverlapEnd( UPrimitiveComponent* pcOverlappedComp, AActor* pcOtherActor,
		UPrimitiveComponent* pcOtherComp, int32 iOtherBodyIndex );

	// Callback function for interacting with the npc
	UFUNCTION()
	void CBInteract();

	// Callback function for removing the widget from the viewport when the player leaves
	UFUNCTION()
	void CBRemoveWidget();

	inline bool IsNarativeOver() {
		return m_bConversationEnded;
	}
};
