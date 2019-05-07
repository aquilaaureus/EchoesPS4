// Fill out your copyright notice in the Description page of Project Settings.

#include "EndedDash.h"
#include "Actors/Kira/EchoesCharacter.h"
#include "Components/SkeletalMeshComponent.h"


void UEndedDash::Notify( USkeletalMeshComponent* pcMeshComp, UAnimSequenceBase* pcAnimation )
{
	AEchoesCharacter* pcPlayer = Cast<AEchoesCharacter>( pcMeshComp->GetOwner() );
	pcPlayer->StopDash();
}