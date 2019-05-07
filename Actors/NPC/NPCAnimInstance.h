// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NPCAnimInstance.generated.h"

//////////////////////////////////////////////////////////////////////////
// Class UNPCAnimInstance: A subclass of UAnimInstance that is used for the NPC
// animation blueprint. It contains variables to be exposed to the animators when
// they create the animation blueprint for the NPC.
//////////////////////////////////////////////////////////////////////////
UCLASS()
class ECHOES_API UNPCAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	// Boolean value to control when the NPC is conversing
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "NPC Animations", DisplayName = "Is Conversing" )
	bool m_bIsConversing;
	
};
