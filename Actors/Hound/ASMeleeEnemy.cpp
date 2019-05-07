// Fill out your copyright notice in the Description page of Project Settings.

#include "ASMeleeEnemy.h"
#include "AdjustedPawnSensingComponent.h"
#include "Actors/Kira/EchoesCharacter.h"
#include "../Mastermind/AMastermind.h"
#include "TimerManager.h"
#include "UObjectGlobals.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "EchoesGameMode.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceConstant.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "Components/ArrowComponent.h"
#include "AIHealthWidgetComponent.h"
#include "Components/WidgetComponent.h"
#include "Animation/HoundAnimInstance.h"
#include "DrawDebugHelpers.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "PlayerComponents/HoundGrappleReciever.h"

//condition check, if health is positive.
bool AASMeleeEnemy::IsAlive()
{
	return m_fHealth>0;
}

AASMeleeEnemy::AASMeleeEnemy( const FObjectInitializer & rObjectInitializer )
	: ACharacter( rObjectInitializer )
	, m_bIsAttacking ( false )
	, m_bIsAttachedToPlayer ( false )
	, m_fAttachmentDelay ( 5.0f )
	, m_fBasicAttackRadius ( 300.0f )
	, m_fGrabRange( 250.0f )
	, m_bShowAttackRange ( false )
	, m_fNotificationReactDistance (3000.0f )
{
	PrimaryActorTick.bCanEverTick = true;

	// Our sensing component to detect players by visibility.
	PawnSensingComp = CreateDefaultSubobject<UAdjustedPawnSensingComponent>( TEXT( "PawnSensingComp" ) );	//create the sensor component
	PawnSensingComp->SetPeripheralVisionAngle( 180.0f );											//The enemies can see you all around
	PawnSensingComp->SightRadius = 2000.0f;															//The distance the enemies can see the player
	
	// Visual Health Widget Component
	m_pcAIHealthWidget = CreateDefaultSubobject<UAIHealthWidgetComponent>( TEXT( "AIHealthWidgetComp" ) );		// Create the Component
	if (m_pcAIHealthWidget)
	{
		m_pcAIHealthWidget->SetWorldLocation(FHealthWidgetPos, false, nullptr, ETeleportType::None);				// Set World Location based on AI position
		m_pcAIHealthWidget->SetRelativeLocation(FVector(0.f, 0.f, 120.f));										// Set Relative Location based on AI position
		m_pcAIHealthWidget->SetComponentTickEnabled(true);														// Enable Component to Tick every frame
		m_pcAIHealthWidget->SetVisibility(true);
		m_pcAIHealthWidget->SetDrawSize(FVector2D(100.f, 30.f));
		m_pcAIHealthWidget->SetupAttachment((USceneComponent*)GetCapsuleComponent());
	}

	m_fHealth = -1.0f;
	m_fMaxHealth = 100.0f;
	m_fMeleeDamage = 20.0f;
	m_fMeleeStrikeCooldown = 1.0f;

	// By default we will not let the AI patrol, we can override this value per-instance.  
	m_fSenseTimeOut = 10.0f;
}

//Called when the game starts or when spawned
void AASMeleeEnemy::BeginPlay()
{
	m_fHealth = m_fMaxHealth;
	Super::BeginPlay();
	
	// Set Up the sight sensor to react with our function
	if ( PawnSensingComp )
	{
		PawnSensingComp->bSeePawns = true;
		PawnSensingComp->OnSeePawn.AddDynamic( this, &AASMeleeEnemy::OnSeePlayer );
	}
	
	if( GetMesh() )
	{
		m_pcAnimInstance = Cast< UHoundAnimInstance >( GetMesh()->GetAnimInstance() );
	}

	m_pcAIHealthWidget->SetVisibility(false);

	m_iMastermindID = AMastermind::GetMasterMind(this)->EnemySpawned(this);
	m_fNotificationReactDistance = m_fNotificationReactDistance * m_fNotificationReactDistance;
	OnEndPlay.AddDynamic( this, &AASMeleeEnemy::RemovedFromPlay );
}

//Called on every loop.
void AASMeleeEnemy::Tick(float fDeltaTime)
{
	Super::Tick(fDeltaTime);

	if (!IsAlive())
	{
		return; //End the update, do nothing else.
	}

	m_fSinceLastAttack += fDeltaTime;

	// Check if the last time we sensed a player is beyond the time out value to prevent bot from endlessly following a player.  
	if (m_bSensedTarget)
	{
		if ((GetWorld()->TimeSeconds - m_fLastSeenTime) > m_fSenseTimeOut)
		{
			m_bSensedTarget = false; //forget that you have seen the target
			
			//forget the target
			m_pcHitActor = nullptr;
			AAIController* pcController = Cast<AAIController>(GetController());
			if (pcController)
			{
				pcController->GetBlackboardComponent()->SetValueAsObject("TargetToFollow", nullptr);
			}
		}
	}

	//Update the Health bar rotation here - not done in component as widget becomes hidden in if TickComponent used.
	UAIHealthWidgetComponent* pcHealthDisplay = Cast<UAIHealthWidgetComponent>(GetComponentByClass(UAIHealthWidgetComponent::StaticClass()));
	if (IsValid(pcHealthDisplay))
	{
		pcHealthDisplay->RotateToFaceCamera();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HealthDisplay is NULL!"))
	}

	//============ AI Tests Compatibility Code ============

	if ( !m_bIsAttachedToPlayer && m_fAttachmentDelay > 0.0f )
	{
		m_fAttachmentDelay -= fDeltaTime;
	}

	//=====================================================

}

//This is called when we see something. This will react to any pawn, so we need to check for what we saw.
void AASMeleeEnemy::OnSeePlayer(APawn * pcPawn)
{

	if (!IsAlive())
	{
		return;
	}

	AEchoesCharacter* SensedPawn = Cast<AEchoesCharacter>(pcPawn);

	if (SensedPawn) //if we have seen a player
	{
		// Keep track of the time the player was last sensed in order to clear the target
		m_fLastSeenTime = GetWorld()->GetTimeSeconds();
		
		if (!m_bSensedTarget)
		{
			m_bSensedTarget = true;
			AMastermind::GetMasterMind(this)->NotifyAliveHounds(pcPawn);
		}
	}
}

//called from mastermind when one hound sees the enemy
void AASMeleeEnemy::NotifyPlayerFound(APawn * pcPawn)
{
	if (!IsAlive())
	{
		return;
	}

	if (FVector::DistSquared(pcPawn->GetActorLocation(), GetActorLocation()) > m_fNotificationReactDistance)
	{
		return;
	}

	AEchoesCharacter* SensedPawn = Cast<AEchoesCharacter>(pcPawn);

	if (SensedPawn) //if we have seen a player
	{
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), pcPawn->GetActorLocation()));
		AAIController* pcController = Cast<AAIController>( GetController() );
		if (pcController)
		{
			pcController->GetBlackboardComponent()->SetValueAsObject("TargetToFollow", SensedPawn);
			pcController->GetBlackboardComponent()->SetValueAsVector("HomeLocation", GetActorLocation());
		}
		// Keep track of the time the player was last sensed in order to clear the target
		m_fLastSeenTime = GetWorld()->GetTimeSeconds();
		m_bSensedTarget = true;
	}

}

//called when the actor is removed from play
void AASMeleeEnemy::RemovedFromPlay(AActor* pcActor, EEndPlayReason::Type EReason)
{
	if (EReason == EEndPlayReason::Destroyed)
	{
		AMastermind::GetMasterMind(this)->EnemyDied(this);
	}
}

//Attack player. This is called from the Behavior Tree (Follower_BT), and runs ALL the tests to see if the player should be attacked.
void AASMeleeEnemy::BasicAttackPlayer()
{
	if (!m_bSensedTarget) 
	{
		return; //If it does not have a target, return.
	}

	if (m_fSinceLastAttack < m_fMeleeStrikeCooldown) //if the cool down period is NOT over
	{
		return;
	}

	//Sphere Trace for Player
	TArray<FHitResult> asHitResults;

	//Start location of the sphere
	FVector sStartLocation = GetActorLocation();

	//End location is just added start + sphere height
	FVector sEndLocation = sStartLocation;
	sEndLocation.Z += 100.0f;

	//Desired object types
	ECollisionChannel eCollisionTypes = ECollisionChannel::ECC_Pawn;

	//Declaring the collision shape (sphere) and it's radius (300.0f)
	FCollisionShape sCollisionShape;
	sCollisionShape.ShapeType = ECollisionShape::Sphere;
	sCollisionShape.SetSphere(m_fBasicAttackRadius);

	//Perform the raycast, returns true if min 1 hit.
	bool bHitSomething = GetWorld()->SweepMultiByChannel(asHitResults, sStartLocation, sEndLocation, FQuat::FQuat(), eCollisionTypes, sCollisionShape);

	//If the raycast hit a number of objects, iterate through them and print their name in the console
	if (bHitSomething)
	{
		for (FHitResult& rsHitOut : asHitResults)
		{
			AEchoesCharacter* pcPlayer = Cast<AEchoesCharacter>(rsHitOut.GetActor());
			if (IsValid(pcPlayer))
			{
				PerformMeleeStrike(pcPlayer); //call ONLY if we are close enough to attack.
				break;
			}
		}
	}

	if (m_bShowAttackRange)
	{
		/*In order to draw the sphere of the first image, I will use the DrawDebugSphere function which resides in the DrawDebugHelpers.h
		This function needs the center of the sphere which in this case is provided by the following equation*/
		FVector sCenterOfSphere = ((sEndLocation - sStartLocation) / 2.0f) + sStartLocation;

		/*Draw the sphere in the viewport*/
		DrawDebugSphere(GetWorld(), sCenterOfSphere, sCollisionShape.GetSphereRadius(), 100, FColor::Green, true, 1.0f);
	}
}

//This is used to attack the player directly. This will perform NO Tests whatsoever.
void AASMeleeEnemy::PerformMeleeStrike(AActor * pcHitActor)
{
	m_fSinceLastAttack = 0.0f;
	m_pcHitActor = pcHitActor; //set the player so that we can deal damage

	if (m_pcAnimInstance)
	{
		m_pcAnimInstance->PlayAttackAnimation();
	}
}

//This is called from animation (using notifies) to directly and correctly time the enemy doing damage.
void AASMeleeEnemy::DealDamageToPlayer()
{
	if( m_pcHitActor && m_pcHitActor != this && IsAlive() )
	{
		AEchoesCharacter* OtherPawn = Cast<AEchoesCharacter>( m_pcHitActor );

		if( OtherPawn )
		{
			// Set to prevent a zombie to attack multiple times in a very short time  
			m_fSinceLastAttack = 0.0f;

			// Deal damage to the hit actor
			UGameplayStatics::ApplyDamage( m_pcHitActor, m_fMeleeDamage, GetInstigatorController(), this, UDamageType::StaticClass() );
			
			if( m_pcAnimInstance )
			{
				m_pcAnimInstance->PlayIdleAnimation();
			}
			
		}
	}
}

//play grabbing animation. Animations is interrupted if grab Succeeds.
void AASMeleeEnemy::JumpAtPlayer()
{
	//Get the player pawn instance from world
	ACharacter* pcPlayerPawn = Cast<ACharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if ( pcPlayerPawn ) 
	{
		if ( m_pcAnimInstance )
		{
			m_pcAnimInstance->PlayGrabbingAnimation();
		}
	}
}

//If the grabbing succeeds, we call this to attach to the player, and tell the player to set up for getting loose from the hound's grip.
void AASMeleeEnemy::AttachToPlayer()
{
	//Get the player pawn instance from world
	AEchoesCharacter* pcPlayerPawn = Cast<AEchoesCharacter>( GetWorld()->GetFirstPlayerController()->GetPawn() );

	//Get the hound grapple component from player if it exists
	UHoundGrappleReciever* pcGrappleReciever = pcPlayerPawn->FindComponentByClass<UHoundGrappleReciever>();

	if (pcPlayerPawn && pcGrappleReciever && pcGrappleReciever->CanBeAttachedTo() )
	{
		//Set actor location to back socket
		FVector sSocketLocation = pcPlayerPawn->GetMesh()->GetSocketLocation( FName( TEXT( "backAttachSocket" ) ) );
		FRotator sLookAtPlayerRot = UKismetMathLibrary::FindLookAtRotation( sSocketLocation, pcPlayerPawn->GetActorLocation() );
		SetActorHiddenInGame( true );
		SetActorEnableCollision( false );

		//Set this actor to rotate Yaw towards player (using lookat)
		SetActorRotation( FRotator( 70.0f, sLookAtPlayerRot.Yaw, GetActorRotation().Roll ) );

		//Attach using receiver
		SetActorLocation( sSocketLocation, true );	
		pcGrappleReciever->AttachHound( this );
		SetActorHiddenInGame( false );

		//Set tracking variable
		m_bIsAttachedToPlayer = true;
		m_fAttachmentDelay = 5.0f;

		//Attack while on back
		if ( m_pcAnimInstance )
		{
			m_pcAnimInstance->PlayAttackAnimation();
		}
	}
}

void AASMeleeEnemy::DetachFromPlayer()
{
	FRotator sNewRotation = GetActorRotation();
	sNewRotation.Pitch = 0.0f;
	SetActorRotation(sNewRotation);
	DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepRelative, true));

	//Resume collision detection
	SetActorEnableCollision(true);

	m_bIsAttacking = false;
	m_bIsAttachedToPlayer = false;
	m_fAttachmentDelay = 5.0f;
}

//Called to stop the enemy from attacking the player (?)
void AASMeleeEnemy::ReturnToIdle()
{
	// Set to prevent a hound to attack multiple times in a very short time  
	m_fSinceLastAttack = 0.0f;

	if (m_pcAnimInstance)
	{
		m_pcAnimInstance->PlayIdleAnimation();
	}
}

//////////////////////////////////////////////////////////////////////////
// Called by the engine when damage is dealt to this actor
//////////////////////////////////////////////////////////////////////////
float AASMeleeEnemy::TakeDamage(float fDamageAmount, FDamageEvent const& krsDamageEvent, AController* pcEventInstigator, AActor* pcDamageCauser)
{
	if (!IsAlive())
	{
		return 0.0f;
	}

	// Decrease Health
	m_fHealth -= fDamageAmount;

	//broadcast the damage event using delegate
	if (m_dEnemyTakeDamageDelegate.IsBound())
	{
		m_dEnemyTakeDamageDelegate.Broadcast(fDamageAmount);
	}

	UE_LOG(LogTemp, Warning, TEXT("HoundHealth: %f || DamageAmount: %f"), m_fHealth, fDamageAmount);

	//Update the health bar on the component.
	if (m_pcAIHealthWidget && (m_fHealth < m_fMaxHealth))
	{
		m_pcAIHealthWidget->SetVisibility(true);
		m_pcAIHealthWidget->UpdateHealthBar(m_fHealth, m_fMaxHealth);
	}

	if (!IsAlive())
	{
		if (m_dEnemyDieDelegate.IsBound())
		{
			//broadcast the die event using delegate
			m_dEnemyDieDelegate.Broadcast(m_iMastermindID);
		}

		if (m_pcAnimInstance)
		{
			m_pcAnimInstance->PlayDeathAnimation();
		}
	}

	return fDamageAmount;
}

//Called to correctly kill this enemy.
//Use for what should be done when this enemy dies.
void AASMeleeEnemy::KillMe()
{

	//============== HOUND AI TESTS RESET ====================

	DetachFromPlayer();

	//========================================================

	Destroy();
	
	///////////////////////////////////////////////////////////////////////////////////////////////////
	// Same part of the code we use after the enemy is destroyed to spawn the pickups in random locations
	// inside a specific range after the enemy is dead
	FVector vManaSpawnPosition;
	vManaSpawnPosition.X = FMath::FRandRange(-100.f, 200.f);
	vManaSpawnPosition.Y = FMath::FRandRange(-100.f, 200.f);
	vManaSpawnPosition.Z = FMath::FRandRange(0.f, 0.f);

	// Spawn the energy item at the owning actors location
	AActor* Mana = GetWorld()->SpawnActor< AActor >(m_pcManaBP, GetActorLocation() + vManaSpawnPosition, GetActorRotation());

	FVector vHealthSpawnPosition;
	vHealthSpawnPosition.X = FMath::FRandRange(200.f, 300.f);
	vHealthSpawnPosition.Y = FMath::FRandRange(200.f, 300.f);
	vHealthSpawnPosition.Z = FMath::FRandRange(0.f, 0.f);

	// Spawn the health item at the owning actors location
	AActor* Health = GetWorld()->SpawnActor< AActor >(m_pcHealthBP, GetActorLocation() + vHealthSpawnPosition, GetActorRotation());
	//
	///////////////////////////////////////////////////////////////////////////////////////////////////

	AMastermind::GetMasterMind(this)->EnemyDied(this);

	//Spawn the broken apart actor (if set in BP)
	//Always spawn because we spawn BEFORE the hound disappears (it will be destroyed at the end of the frame, but NOW it STILL is there)
	if (m_pcBrokenUpMesh)
	{
		FActorSpawnParameters params;
		params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		GetWorld()->SpawnActor<AActor>(m_pcBrokenUpMesh, GetActorLocation(), GetActorRotation(), params);
	}
}

