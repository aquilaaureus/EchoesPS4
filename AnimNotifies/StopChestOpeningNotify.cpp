// Fill out your copyright notice in the Description page of Project Settings.

#include "StopChestOpeningNotify.h"
#include "Actors/Kira/EchoesCharacter.h"
#include "Components/SkeletalMeshComponent.h"

void UStopChestOpeningNotify::Notify ( USkeletalMeshComponent* pcMeshComp , UAnimSequenceBase* pcAnimation )
{
	AEchoesCharacter* pcPlayer = Cast<AEchoesCharacter> ( pcMeshComp->GetOwner ( ) );
	pcPlayer->SetOpeningChestAnimState(false);
}


