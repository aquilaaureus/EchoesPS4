// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "FootprintNotify.generated.h"

class USkeletalMeshSocket;
struct FHitResult;

UCLASS()
class ECHOES_API UFootprintNotify : public UAnimNotify
{
	GENERATED_BODY()

private:
	UPROPERTY( EditAnywhere, Category = "Anim Notify", DisplayName = "Bone Name" )
	FName m_sBoneName;	
	
	void Trace( FHitResult& cOutHit, const FVector& vLocation, AActor* pcIgnore, UWorld* pcWorld ) const;

protected:
	virtual void Notify( USkeletalMeshComponent* pcMeshComp, UAnimSequenceBase* pcAnimation ) override;
};
