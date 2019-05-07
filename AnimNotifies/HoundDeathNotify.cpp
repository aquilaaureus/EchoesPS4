// Fill out your copyright notice in the Description page of Project Settings.

#include "HoundDeathNotify.h"
#include "Actors/Hound/ASMeleeEnemy.h"
#include <Components/SkeletalMeshComponent.h>

void UHoundDeathNotify::Notify(USkeletalMeshComponent * pcMeshComp, UAnimSequenceBase * pcAnimation)
{
	AASMeleeEnemy* pcHound = Cast<AASMeleeEnemy>(pcMeshComp->GetOwner());
	if (pcHound)
	{
		pcHound->KillMe();
	}
}
