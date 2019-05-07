// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

//Main include - needs to be first header
#include "EchoesCharacter.h"

//Inclusive headers
#include "../Hound/ASMeleeEnemy.h"
#include "Actors/NPC/NPC.h"
#include "Animation/PlayerAnimInstance.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraShake.h"
#include "Components/ActorComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CoreGlobals.h"
#include "DrawDebugHelpers.h"
#include "EchoesGameMode.h"
#include "Engine.h"
#include "EngineUtils.h"
#include "Engine/EngineTypes.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/SpringArmComponent.h"
#include "GCFramework/GCStaticHelperLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetInputLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "LevelScripts/PS4BuildMapLS.h"
#include "Materials/MaterialExpressionParameter.h"
#include "NoExportTypes.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "PlayerComponents/DashGhostComponent.h"
#include "PlayerComponents/HoundGrappleReciever.h"
#include "PlayerComponents/PlayerStatsComponent.h"
#include "Runtime/Engine/Classes/Components/InputComponent.h"
#include "TimerManager.h"
#include "UISystems/HUD/PlayerStatsUserWidget.h"
#include "UISystems/Widgets/MenuWidgetPlayerController.h"
#include "UISystems/Menus/Map_Screen/MapAnnotation.h"
#include "UISystems/Menus/Map_Screen/MapScreen.h"
#include "UObjectIterator.h"

//////////////////////////////////////////////////////////////////////////
// AEchoesCharacter

AEchoesCharacter::AEchoesCharacter() 
: m_fBaseTurnRate							( 45.0f )
, m_fBaseLookUpRate							( 45.0f )
, m_sCameraWorldRot							( FRotator ( -30.0f , 0.0f , 0.0f ) )
, m_fSpringArmLength						( 1200.0f )
, m_fCameraFOV								( 75.0f )
, m_bIsCameraLocked							( false )
, m_pcPlayerCameraShake						( nullptr )
, m_fMaxShakeScale							( 0.05f )
, m_fShakeScaleMultiplier					( 0.01f )
, m_fHoldDurationBeforeShaking				( 0.1 )
, m_bIsChargedAttackPressed					( false )
, m_bChargedAttackAcquired					( false )
, m_fCurrentMeleePressedDuration	        ( 0.f )
, m_fThresholdDurationForHeavyMelee			( 2.0f )
, m_fAttackWindowCurrentTime				( 0.0f )
, m_bIsPlayerTrapped						( false )
, m_bIsAttacking							( false )
, m_bTriggerNextAttack						( false )
, m_iComboAttackCount                       ( 0 )
, m_bIsNextAttackWindowOpen					( false )
, m_pcMeleePatricleEmitter1					( nullptr )
, m_pcMeleePatricleEmitter2					( nullptr )
, m_pcMeleePatricleEmitter3					( nullptr )
, m_eComboAttackEnum						( EComboAttack::ComboAttackOne )
, m_pcStunEncasementActorClass				( nullptr )
, m_pcStunEncasementActorInst				( nullptr )
, m_pcHoundGrappleReciever					( nullptr )
, m_pcPlayerStatsComponent					( nullptr )
, m_sRespawnLocation						( FVector::ZeroVector )
, m_bCanDoubleJump							( false )
, m_bCanDash								( true )
, m_bEnergyUpgrade							( false )
, m_bHealthUpgrade							( false )
, m_iCurrentJump							( 0 )
, m_bDashAquired							( false )
, m_pcMenuWidgetController					( nullptr )
, m_pcInput									( nullptr )
, m_iHealthLowTimeCounter					( 0 )
, m_fLowHealWarnLimit						( 50.0f )
, m_pcLowHealthPPMat						( nullptr )
, m_bIsJumpBound							( false )
, m_eCurrentPlayerState						( EPlayerState::Idle )
, m_pcCameraBoom							( nullptr )
, m_pcFollowCamera							( nullptr )
, m_vStartingLocation						( FVector::ZeroVector )
, m_iCurrentPauseMenuIndex					( 0 )
, m_bChangedPauseIndex						( false )
, m_bRemoveDeathBinding						( false )
, m_pcDeathWidgetInstance					( nullptr )
, m_pcDeathWidget							( nullptr )
, m_fDashCooldown							( 1.0f )
, m_fJumpCheckTime							( 0.09f )
, m_fUpgradeTime                            ( 2.f )
, m_fDashDistance                           (1000.0f )
, m_vDashStartPoint                         ( FVector::ZeroVector )
, m_vDashEndPoint                           ( FVector::ZeroVector )
, m_pcLightAttackSound						( nullptr )
, m_pcHeavyAttackSound						( nullptr )
, m_pcSingleJumpSound                       ( nullptr )
, m_pcDoubleJumpSound                       ( nullptr )
, m_apcFlinchSound							( nullptr )
, m_pcDeathSound							( nullptr )
, m_pcFootStepSound							( nullptr )
, m_ffadecounter							( -1.0f )
, m_pcBlackScreenPPMat						( nullptr )
, m_bGameFinish								( false )
, m_pcNPC									( nullptr )
, m_fFadeTimeDilation						( 1.0f )
, m_pcNpc_BP								( nullptr )
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize( 42.f, 96.0f );

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator( 0.0f, 540.0f, 0.0f ); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Setting up the box collider for the melee attack.
	FVector BoxExtends = FVector( 81.f, 69.f, 76.f );
	FVector RelativeLocation = FVector( 100.f, 0.f, 0.f );

	// Create a camera boom (pulls in towards the player if there is a collision)
	m_pcCameraBoom = CreateDefaultSubobject<USpringArmComponent>( TEXT( "CameraBoom" ) );
	m_pcCameraBoom->SetupAttachment( RootComponent );
	m_pcCameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	m_pcCameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	m_pcFollowCamera = CreateDefaultSubobject<UCameraComponent>( TEXT( "FollowCameraComponent" ) );
	m_pcFollowCamera->SetupAttachment( m_pcCameraBoom, USpringArmComponent::SocketName ); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	m_pcFollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//Add hound grapple component
	m_pcHoundGrappleReciever = CreateDefaultSubobject<UHoundGrappleReciever>( TEXT( "HoundGrappleReciever" ) );

	//Add player stats component
	m_pcPlayerStatsComponent = CreateDefaultSubobject<UPlayerStatsComponent>( TEXT( "PlayerStatsComponent" ) );

	//create and add the ghost component
	m_pcDashGhostComp = CreateDefaultSubobject<UDashGhostComponent>(TEXT("DashGhostComponent"));
}

void AEchoesCharacter::OnPickAxeOverlap( UPrimitiveComponent* pcHitComponent, AActor* pcOtherActor,
	UPrimitiveComponent* pcOtherComp, int32 iOtherBodyIndex, bool bFromSweep, const FHitResult & rSweepResult )
{
	if( pcOtherComp->ComponentTags.Num() > 0 )
	{
		if( pcOtherComp->ComponentTags[ 0 ].IsEqual( "PlayerDamageable" ) )
		{
			FVector sLocation = pcOtherActor->GetActorLocation();
			sLocation.Z = GetActorLocation().Z;

			FRotator vParticleRotation = ( sLocation - GetActorLocation() ).Rotation();
			vParticleRotation.Pitch -= 90.0f;						// Rotate it by 90 degree to face enemy
			float fDamageMultiplier = 1.0f;							// Damage Multiplier for each attack defaulted to one but designers can change

			UParticleSystem* pcMeleeParticle = nullptr;

			// If the melee attack combo is the first hit then the VFX of that hit is playing
			if( m_eComboAttackEnum == ComboAttackOne )
			{
				fDamageMultiplier = m_pcPlayerStatsComponent->GetDamageMultiplier( 1 );
				pcMeleeParticle = m_pcMeleePatricleEmitter1;
			}
			// If the melee attack combo is the second hit then the VFX of that hit is playing
			else if( m_eComboAttackEnum == ComboAttackTwo )
			{
				fDamageMultiplier = m_pcPlayerStatsComponent->GetDamageMultiplier( 2 );
				pcMeleeParticle = m_pcMeleePatricleEmitter2;
			}
			// If the melee attack combo is the third hit then the VFX of that hit is playing
			else
			{
				fDamageMultiplier = m_pcPlayerStatsComponent->GetDamageMultiplier( 3 );
				pcMeleeParticle = m_pcMeleePatricleEmitter3;
			}

			if( pcMeleeParticle )
			{
				pcMeleeParticle->AddToRoot();
				UGameplayStatics::SpawnEmitterAtLocation( GetWorld(), pcMeleeParticle,
					sLocation, vParticleRotation, true );
			}

			UGameplayStatics::ApplyDamage( pcOtherActor, m_pcPlayerStatsComponent->GetPlayerDamage() * fDamageMultiplier, nullptr, this, UDamageType::StaticClass() );
		}
	}
}

/*bool AEchoesCharacter::ProcessConsoleExec ( const TCHAR * Cmd, FOutputDevice & Ar, UObject * Executor )
{
bool Result = false;
TArray<UActorComponent*> Components;
this->GetComponents<UActorComponent> ( Components );
for( UActorComponent* Comp : Components )
{
Result = Comp->ProcessConsoleExec ( Cmd, Ar, Executor ) || Result;
}
return Result;
}*/

void AEchoesCharacter::BecomeStunned( float fStunDuration, bool bIsInputDelimited )
{
	UCharacterMovementComponent* pcPlayerMoveComp = Cast<UCharacterMovementComponent>( GetMovementComponent() );
	if( pcPlayerMoveComp )
	{
		pcPlayerMoveComp->Velocity = FVector::ZeroVector;
		APlayerController* pcPlayerController = Cast<APlayerController>( GetController() );
		if( pcPlayerController )
		{
			//Disable player inputs and pause animations
			m_bIsPlayerTrapped = true;
			GetMesh()->bPauseAnims = true;
		}
		else { UE_LOG( LogTemp, Error, TEXT( "PlayerController was invalid when trying to stun player!" ) ); }

	}
	else { UE_LOG( LogTemp, Error, TEXT( "PlayerMovementComponent was invalid when trying to stun player!" ) ); }

	//Spawn the encasement actor if it's valid
	if( !m_pcStunEncasementActorInst && m_pcStunEncasementActorClass )
	{
		m_pcStunEncasementActorInst = GetWorld()->SpawnActor<AActor>( m_pcStunEncasementActorClass, GetActorLocation(), GetActorRotation() );
		m_pcStunEncasementActorInst->AttachToActor( this, FAttachmentTransformRules( EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true ) );
	}

	//Input delimited behavior
	if( bIsInputDelimited )
	{
		m_pcHoundGrappleReciever->ForceGrappleBehaviour();

		//Set looping callback for timed check
		GetWorld()->GetTimerManager().SetTimer( m_sStunCBTimer, this, &AEchoesCharacter::CheckStunEnded, 0.25f, true );
	}
	//Timed stun behavior
	else
	{
		//Set a non-looping timer (for callback)
		GetWorld()->GetTimerManager().SetTimer( m_sStunCBTimer, this, &AEchoesCharacter::CBStunRelease, fStunDuration, false );
	}
}

void AEchoesCharacter::CheckStunEnded()
{
	if( m_pcHoundGrappleReciever )
	{
		if( m_pcHoundGrappleReciever->CanBeAttachedTo() )
		{
			GetWorld()->GetTimerManager().ClearTimer( m_sStunCBTimer );
			CBStunRelease();
		}
	}
}

void AEchoesCharacter::CBStunRelease()
{
	APlayerController* pcPlayerController = Cast<APlayerController>( GetController() );
	if( pcPlayerController )
	{
		//Destroy the encasement actor if it exists.
		if( m_pcStunEncasementActorInst )
		{
			m_pcStunEncasementActorInst->Destroy();
			m_pcStunEncasementActorInst = nullptr;
		}

		//Disable the pickaxe collider - we may have been mid attack when stunned (it's added in bp so we're gonna check for it)
		USphereComponent* pcPickaxeCollider = Cast<USphereComponent>( GetComponentByClass( USphereComponent::StaticClass() ) );
		if( pcPickaxeCollider )
		{
			pcPickaxeCollider->SetCollisionEnabled( ECollisionEnabled::NoCollision );
		}

		//Re-enable player inputs and stop animations
		m_bIsPlayerTrapped = false;
		StopAnimMontage();

		//Reset attack variables (so player can attack again)
		m_bIsAttacking = false;
		m_bTriggerNextAttack = false;
		m_bIsNextAttackWindowOpen = false;

		//Allow animations to play again
		GetMesh()->bPauseAnims = false;

		//In the case of reset, the grapple wouldn not have been reset - call here for check
		m_pcHoundGrappleReciever->DetachHound();
	}
	else { UE_LOG( LogTemp, Error, TEXT( "PlayerController was invalid when trying to stun release player!" ) ); }
}

//////////////////////////////////////////////////////////////////////////
// Starts a countdown timer to check for double click input before executing normal dash
//////////////////////////////////////////////////////////////////////////
void AEchoesCharacter::CBDash()
{
	//Prevent input if the player is trapped
	if( !m_bIsPlayerTrapped )
	{
		// Start a timer to wait for a second input before executing a single dash
		if( !m_bIsAttacking && m_bDashAquired && m_bCanDash )
		{
			// Make sure the player has enough stamina to dash
			if( m_pcPlayerStatsComponent->GetPlayerCurrentEnergy() >= m_pcPlayerStatsComponent->GetPlayerDashCost() )
			{
				// Reduce stamina by dash cost
				m_pcPlayerStatsComponent->AddEnergy( -1.0f * m_pcPlayerStatsComponent->GetPlayerDashCost() );

				// Dash forward
				DashForward( );
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Function to actually make the player dash forward in their facing direction
//////////////////////////////////////////////////////////////////////////
void AEchoesCharacter::DashForward ( )
{

	m_pcDashGhostComp->Activate();

	//Dash is handled in blueprint, trigger the delegate
	if ( m_dPlayerPerformDashDelegate.IsBound ( ) )
	{
		////////////////////////////////////////////////////////////////////////////
		//// Vector Logic 

		//// Set the starting point of the dash to the current position of the player 
		//m_vDashStartPoint = GetActorLocation();

		//// Truncate the float value of m_fDashDistance into a whole integer rounded down
		//int iDashDistTrunc = trunc( m_fDashDistance );

		//// Set the Capsule Component's Forward Vector into a temp container
		//FVector vCapsuleCompForwardVector = GetCapsuleComponent()->GetForwardVector();

		//// Multiply this temp container by the value of iDashDistTrunc
		//FVector vTempSphereEnd = vCapsuleCompForwardVector * iDashDistTrunc;

		//// Determine the end point of the Sphere Trace
		//FVector vSphereTraceEnd = GetActorLocation() + vTempSphereEnd;

		//// Vector Logic
		////////////////////////////////////////////////////////////////////////////
		//
		//// Get the Radius of the Capsule Component of the player
		//float fSphereRadius = GetCapsuleComponent()->GetUnscaledCapsuleRadius();

		//// Create container to use with SphereTraceSingle
		//FHitResult sHitResult;

		//// SphereTraceSingle detects for collisions in a sphere radius along a specified distance. This func will return true if there is a collision, else returns false
		//// TODO: Change ETraceTypeQuery::TraceTypeQuery_MAX to the appropriate value - This is stopping appropriate collisions
		//// TODO: Change value of  EDrawDebugTrace::Persistent to obtain different behaviours for debug drawing - Change this to None when complete
		//bool bSphereHit = UKismetSystemLibrary::SphereTraceSingle( this, m_vDashStartPoint, m_vDashEndPoint, fSphereRadius, ETraceTypeQuery::TraceTypeQuery_MAX, false, TArray<AActor*>() , EDrawDebugTrace::Persistent, sHitResult, true, FLinearColor::Red, FLinearColor::Green, 5.f );

		//UE_LOG( LogTemp, Warning, TEXT( "Value of vSphereTraceEnd is %s" ), *vSphereTraceEnd.ToString() );

		//// Check if there is a collision with a world object
		//// If there isn't perform the full distance of the dash
		//if ( bSphereHit )
		//{
		//	m_vDashEndPoint = sHitResult.Location;
		//	UE_LOG( LogTemp, Warning, TEXT( "The location of the collision occurs at %s" ), *sHitResult.Location.ToString() );
		//}
		//else
		//{
		//	m_vDashEndPoint = vSphereTraceEnd;
		//	UE_LOG( LogTemp, Warning, TEXT( "No Collision has occured" ) );
		//}

		// Broadcast the animation for the dash
		m_dPlayerPerformDashDelegate.Broadcast();
	}

	m_bCanDash = false;
	FTimerHandle sTimer;
	GetWorld ( )->GetTimerManager ( ).SetTimer ( sTimer , this , &AEchoesCharacter::CBResetDash , m_fDashCooldown , false );

	UPlayerAnimInstance* pcAnimInstance = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	if (pcAnimInstance)
	{
		pcAnimInstance->m_bCanDash = true;
	}
}

//////////////////////////////////////////////////////////////////////////
// Callback function to reset dash enabled after a cooldown
//////////////////////////////////////////////////////////////////////////
void AEchoesCharacter::CBResetDash()
{
	m_bCanDash = true;
	UPlayerAnimInstance* pcAnimInstance = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	if (pcAnimInstance) 
	{
		pcAnimInstance->m_bCanDash = false;
	}
}

//////////////////////////////////////////////////////////////////////////
// Sets whether the player has unlocked the dash ability
//////////////////////////////////////////////////////////////////////////
void AEchoesCharacter::SetDashEnabled()
{
	InputComponent->BindAction( "Dash", IE_Pressed, this, &AEchoesCharacter::CBDash );
	m_bDashAquired = true;
}

//////////////////////////////////////////////////////////////////////////
// Callback function for stamina upgrade pressed
//////////////////////////////////////////////////////////////////////////
void AEchoesCharacter::CBEnergyUpgradePressed()
{
	if( m_pcPlayerStatsComponent->GetResourceAmount() >= m_pcPlayerStatsComponent->GetUpgradeRequired() && !m_bHealthUpgrade )
	{
		m_bEnergyUpgrade = true;

		GetWorld()->GetTimerManager().SetTimer( m_sUpgradeTimer, this, &AEchoesCharacter::CBUpgradeMaxEnergy, m_fUpgradeTime, false );
	}
}

//////////////////////////////////////////////////////////////////////////
// Callback function for stamina upgrade released
//////////////////////////////////////////////////////////////////////////
void AEchoesCharacter::CBEnergyUpgradeReleased()
{
	if( GetWorld()->GetTimerManager().TimerExists( m_sUpgradeTimer ) )
	{
		GetWorld()->GetTimerManager().ClearTimer( m_sUpgradeTimer );
		m_bEnergyUpgrade = false;
	}
}

//////////////////////////////////////////////////////////////////////////
// Callback function for health upgrade pressed
//////////////////////////////////////////////////////////////////////////
void AEchoesCharacter::CBHealthUpgradePressed()
{
	if( m_pcPlayerStatsComponent->GetResourceAmount() >= m_pcPlayerStatsComponent->GetUpgradeRequired() && !m_bEnergyUpgrade )
	{
		m_bHealthUpgrade = true;

		GetWorld()->GetTimerManager().SetTimer( m_sUpgradeTimer, this, &AEchoesCharacter::CBUpgradeMaxHealth, m_fUpgradeTime, false );
	}
}

//////////////////////////////////////////////////////////////////////////
// Callback function for health upgrade released
//////////////////////////////////////////////////////////////////////////
void AEchoesCharacter::CBHealthUpgradeReleased()
{
	if( GetWorld()->GetTimerManager().TimerExists( m_sUpgradeTimer ) )
	{
		GetWorld()->GetTimerManager().ClearTimer( m_sUpgradeTimer );
		m_bHealthUpgrade = false;
	}
}

//////////////////////////////////////////////////////////////////////////
// Callback function to upgrade max energy
//////////////////////////////////////////////////////////////////////////
void AEchoesCharacter::CBUpgradeMaxEnergy()
{
	m_bEnergyUpgrade = false;
	m_pcPlayerStatsComponent->UpgradeMaxEnergy();
}

//////////////////////////////////////////////////////////////////////////
// Callback function to upgrade max health
//////////////////////////////////////////////////////////////////////////
void AEchoesCharacter::CBUpgradeMaxHealth()
{
	m_bHealthUpgrade = false;
	m_pcPlayerStatsComponent->UpgradeMaxHealth();
}

//////////////////////////////////////////////////////////////////////////
// Sets whether the player has unlocked the charged attack ability
//////////////////////////////////////////////////////////////////////////
void AEchoesCharacter::SetChargedAttackEnabled()
{
	m_bChargedAttackAcquired = true;

	//InputComponent->BindAction( "MeleeAttack", IE_Pressed, this, &AEchoesCharacter::StartMeleeAttackPressedDurationMeasuring );
	//InputComponent->BindAction( "MeleeAttack", IE_Released, this, &AEchoesCharacter::StopMeleeAttackPressedDurationMeasuring );

	InputComponent->BindAction ( "ChargedAttack" , IE_Pressed , this , &AEchoesCharacter::StartChargedAttackPressedDurationMeasuring );
	InputComponent->BindAction ( "ChargedAttack" , IE_Released , this , &AEchoesCharacter::StopChargedAttackPressedDurationMeasuring );
	InputComponent->BindAction ( "MeleeAttack" , IE_Pressed , this , &AEchoesCharacter::ChargedAttack );
}

//////////////////////////////////////////////////////////////////////////
// Input

void AEchoesCharacter::SetupPlayerInputComponent( class UInputComponent* PlayerInputComponent )
{
	// Set up game play key bindings
	check( PlayerInputComponent );
	m_pcInput = PlayerInputComponent;
	PlayerInputComponent->ClearActionBindings();

	// Set up for Movement Ability input bindings for scheme one
	PlayerInputComponent->BindAction( "Jump", IE_Pressed, this, &AEchoesCharacter::CBJump );
	PlayerInputComponent->BindAction( "Jump", IE_Released, this, &ACharacter::StopJumping );

	// Set up for Melee attack Player input bindings
	PlayerInputComponent->BindAction( "MeleeAttack", IE_Pressed, this, &AEchoesCharacter::SingleClickAttack );
	PlayerInputComponent->BindAction( "ToggleCameraLock", IE_Pressed, this, &AEchoesCharacter::LockCamera );

	// Setup upgrade input
	PlayerInputComponent->BindAction( "UpgradeHealth", IE_Pressed, this, &AEchoesCharacter::CBHealthUpgradePressed );
	PlayerInputComponent->BindAction( "UpgradeHealth", IE_Released, this, &AEchoesCharacter::CBHealthUpgradeReleased );

	PlayerInputComponent->BindAction( "UpgradeEnergy", IE_Pressed, this, &AEchoesCharacter::CBEnergyUpgradePressed );
	PlayerInputComponent->BindAction( "UpgradeEnergy", IE_Released, this, &AEchoesCharacter::CBEnergyUpgradeReleased );

	PlayerInputComponent->BindAxis( "MoveForward", this, &AEchoesCharacter::MoveForward );
	PlayerInputComponent->BindAxis( "MoveRight", this, &AEchoesCharacter::MoveRight );

	m_bIsJumpBound = true;

	//Add the hound grapple input binding for the component.
	if( m_pcHoundGrappleReciever )
	{
		m_pcHoundGrappleReciever->AddRightStickBinding( m_pcInput );
	}
}
// Input
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Control Scheme 2 functions
void AEchoesCharacter::SingleClickJump()
{
	// set a timer to check jump timing
	GetWorld()->GetTimerManager().SetTimer( m_fthJumpCheckTimeHandle, this, &AEchoesCharacter::CBJump, m_fJumpCheckTime, false );
}

//////////////////////////////////////////////////////////////////////////
// Called when the player presses the input for the jump action
//////////////////////////////////////////////////////////////////////////
void AEchoesCharacter::CBJump()
{
	//Prevent input if the player is trapped
	if( !m_bIsPlayerTrapped && !m_bIsChargedAttackPressed )
	{
		// If the player is on the ground/hasn't jumped yet, jump
		if( m_iCurrentJump == 0 )
		{
			// Make sure the player has enough stamina to jump
			if ( m_pcPlayerStatsComponent->GetPlayerCurrentEnergy ( ) >= m_pcPlayerStatsComponent->GetPlayerJumpCost ( ) )
			{
				// Reduce stamina by jump cost
				m_pcPlayerStatsComponent->AddEnergy ( -1.0f * m_pcPlayerStatsComponent->GetPlayerJumpCost ( ) );

				ACharacter::Jump ( );
				++m_iCurrentJump;

				//play single jump sound
				if ( m_pcSingleJumpSound )
				{
					UGameplayStatics::PlaySoundAtLocation ( this , m_pcSingleJumpSound , GetActorLocation ( ) );
				}
			}
		}
		// If the player has jumped and has unlocked double jump, force them a bit higher
		else if( m_iCurrentJump == 1 && m_bCanDoubleJump )
		{
			// Make sure the player has enough stamina to double jump
			if ( m_pcPlayerStatsComponent->GetPlayerCurrentEnergy ( ) >= m_pcPlayerStatsComponent->GetPlayerDoubleJumpCost ( ) )
			{
				// Reduce stamina by double jump cost
				m_pcPlayerStatsComponent->AddEnergy ( -1.0f * m_pcPlayerStatsComponent->GetPlayerDoubleJumpCost ( ) );
				LaunchCharacter ( FVector ( 0.f , 0.f , m_pcPlayerStatsComponent->GetDoubleJumpForce() ) , false , true );
				++m_iCurrentJump;
				UPlayerAnimInstance* pcAnimInstance = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
				if (pcAnimInstance) 
				{
					pcAnimInstance->m_bIsDoubleJumping = true;
				}

				//play double jump sound
				if ( m_pcDoubleJumpSound )
				{
					UGameplayStatics::PlaySoundAtLocation ( this , m_pcDoubleJumpSound , GetActorLocation ( ) );
				}
			}
		}
	}
}

void AEchoesCharacter::SingleClickAttack()
{
	//check if the player is grounded or not
	//if not, return because she should not attack in air
	if ( GetMovementComponent ( )->IsFalling ( ) )
	{
		return;
	}

	//Prevent input if the player is trapped
	if ( !m_bIsPlayerTrapped && !m_bChargedAttackAcquired )
	{
		// Make sure the player has enough stamina to attack
		if ( m_pcPlayerStatsComponent->GetPlayerCurrentEnergy ( ) >= m_pcPlayerStatsComponent->GetPlayerAttackCost ( ) )
		{
			CBAttackCall ( );
		}
	}

}

void AEchoesCharacter::ChargedAttack ( )
{
	GetWorld ( )->GetFirstPlayerController ( )->PlayerCameraManager->StopAllCameraShakes ( true );
	m_bIsChargedAttackPressed = false;
	if ( m_fCurrentMeleePressedDuration > m_fThresholdDurationForHeavyMelee )
	{
		m_fCurrentMeleePressedDuration = m_fThresholdDurationForHeavyMelee;
	}

	// Make sure the player has enough stamina to attack
	if ( m_pcPlayerStatsComponent->GetPlayerCurrentEnergy ( )>=m_pcPlayerStatsComponent->GetPlayerAttackCost ( ) )
	{
		m_pcPlayerStatsComponent->SetDamageMultiplier ( 1.0f + m_fCurrentMeleePressedDuration , 1 );
		CBAttackCall ( );
	}

	//broadcast the delegate to stop all the particle effects on pickaxe
	if ( m_dPlayerStopChargedDelegate.IsBound ( ) )
	{
		m_dPlayerStopChargedDelegate.Broadcast ( );
	}

	m_fCurrentMeleePressedDuration = 0.0f;
}

void AEchoesCharacter::CBAttackCall ( )
{

	if ( m_dPlayerAttackInputDelegate.IsBound())
	{
		m_dPlayerAttackInputDelegate.Broadcast ( );
	}

}

void AEchoesCharacter::CheckForNextAttack ( )
{
	if (m_bTriggerNextAttack)
	{
		m_bTriggerNextAttack = false;
		CBAttackCall ( );
	}
	else
	{
		//player didn't request for next attack, set attack count to 0 and isAttack to 
		m_iComboAttackCount = 0;
		m_bIsAttacking = false;
	}
}

void AEchoesCharacter::StopDash()
{
	UPlayerAnimInstance* pcAnimInstance = Cast<UPlayerAnimInstance>( GetMesh()->GetAnimInstance() );
	if( pcAnimInstance )
	{
		pcAnimInstance->m_bCanDash = false;
	}
}

void AEchoesCharacter::BeginPlay()
{
	Super::BeginPlay();

	m_bGameFinish = false;

	m_pcDynamicOverlay = UMaterialInstanceDynamic::Create(m_pcBlackScreenPPMat, this);

	m_ffadecounter = 1.0f;
	if (m_pcFollowCamera && m_pcDynamicOverlay)
	{
		m_pcDynamicOverlay->SetScalarParameterValue("Weight", m_ffadecounter);
		m_pcFollowCamera->AddOrUpdateBlendable(m_pcDynamicOverlay);
	}

	UPlayerAnimInstance* pcAnimInstance = Cast<UPlayerAnimInstance>( GetMesh()->GetAnimInstance() );
	if( pcAnimInstance )
	{
		pcAnimInstance->m_bAttackOneFinished = true;
	}

	m_eCurrentPlayerState = Idle;
	TArray<USceneComponent*> Components;
	GetMesh()->GetChildrenComponents( false, Components );

	//Adding pickaxe overlap collision call by searching for static mesh component named 'Pickaxe'
	for( int32 i = 0; i < Components.Num(); i++ )
	{
		UStaticMeshComponent* pcStaticMeshComponent = Cast<UStaticMeshComponent>( Components[ i ] );

		if( pcStaticMeshComponent->GetName().Equals( FName( "Pickaxe" ).ToString() ) )
		{
			//Get it's collision component
			UPrimitiveComponent* pcPickAxeCollisionComponent = Cast<UPrimitiveComponent>( pcStaticMeshComponent->GetChildComponent( 0 ) );
			if( pcPickAxeCollisionComponent )
			{
				pcPickAxeCollisionComponent->OnComponentBeginOverlap.AddDynamic( this, &AEchoesCharacter::OnPickAxeOverlap );
			}
		}
	}

	Controller->SetControlRotation( m_sCameraWorldRot );
	m_pcCameraBoom->TargetArmLength = m_fSpringArmLength;
	m_pcFollowCamera->FieldOfView = m_fCameraFOV;
	m_vStartingLocation = GetActorLocation();

	if (pcAnimInstance)
	{
		pcAnimInstance->m_bAttackOneFinished = true;
	}
	//Set player controller pointer
	m_pcMenuWidgetController = Cast<AMenuWidgetPlayerController>( GetWorld()->GetFirstPlayerController() );

	m_bDashAquired = false;
}

void AEchoesCharacter::Tick(float fDeltaTime)
{
	Super::Tick(fDeltaTime);

	if (m_bGameFinish)
	{
		switch (m_iStage)
		{

		case 1: //Stage 1: Fade to black (lerp material input 1) in [1,0]
			m_ffadecounter -= fDeltaTime * m_fFadeTimeDilation;
			if (m_ffadecounter < 0.0f)
			{
				m_ffadecounter = 0.0f;
				++m_iStage;
			}
			m_pcDynamicOverlay->SetScalarParameterValue("Weight", m_ffadecounter);
			break;
		case 2: //Stage 2: Spawn NPC and set actor pos
			GetMesh()->SetRenderCustomDepth(false); //disable silhouette
			SetActorLocation(FVector(7590.0f, 520.0f, 1856.0f));
			m_pcNPC = GetWorld()->SpawnActor<ANPC>(m_pcNpc_BP, FVector(7800.0f, 550.0f, 1780.0f), FRotator(0.0f, 90.0f, 0.0f), FActorSpawnParameters());

			if (!m_pcNPC)
			{
				m_iStage = -1;
				UE_LOG(LogTemp, Error, TEXT("NPC SPAWN FAILED!!!!!!!!!!"));
				break;
			}

			SetActorRotation(UKismetMathLibrary::FindLookAtRotation(FVector(7590.0f, 520.0f, 0.0f), FVector(7800.0f, 550.0f, 0.0f)));
			GetCharacterMovement()->DisableMovement();
			++m_iStage;
			break;
		case 3: //Fade from black to screen (lerp material input 2) [0,1]
			m_ffadecounter += fDeltaTime * m_fFadeTimeDilation;
			if (m_ffadecounter >= 0)
			{
				if (m_ffadecounter >= 1.0f)
				{
					m_ffadecounter = 1.0f;
					++m_iStage;
				}
				m_pcDynamicOverlay->SetScalarParameterValue("Weight", m_ffadecounter);
			}
			break;
		case 4: //Check for Dialogue End
			if (m_pcNPC->IsNarativeOver())
			{
				m_ffadecounter = 0.0f; //Difference between this and 1 is how long we wait before we start fading to white
				++m_iStage;
			}
			break;
		case 5: //fade to white (lerp material input 3) [1,2] and then logo (lerp material input 4) [2,3], then hold logo for one second [3,4]
			m_ffadecounter += fDeltaTime * m_fFadeTimeDilation;
			if (m_ffadecounter > 1.0f)
			{
				m_pcDynamicOverlay->SetScalarParameterValue("Weight", m_ffadecounter);

				if (m_ffadecounter > 4.0f)
				{
					m_ffadecounter = 1.0f;
					++m_iStage;
				}
			}
			break;
		case 6: //exit and load main menu
			if (m_pcNPC)
			{
				m_pcNPC->Destroy();
			}
			if( m_pcCreditsWidget )
			{
				if( !m_pcCreditsWidgetInst )
				{
					m_pcCreditsWidgetInst = CreateWidget<UUserWidget>( UGameplayStatics::GetGameInstance( this ), m_pcCreditsWidget );
					if( m_pcCreditsWidgetInst )
					{
						m_pcCreditsWidgetInst->AddToViewport( 100 );
						++m_iStage;
					}
				}
			}
			break;
		default:
			break;
		}

		return; //Do not proceed past this point
	}

	// If the player is currently dead
	if ( m_bRemoveDeathBinding )
	{
		// Check if the player has pressed the X button or Spacebar to initiate the Reset call
		// Must be done this way in order to disable all input when the player is dead
		if ( m_pcMenuWidgetController->IsInputKeyDown( EKeys::Gamepad_FaceButton_Bottom ) || m_pcMenuWidgetController->IsInputKeyDown( EKeys::SpaceBar ) )
		{
			Reset();

			// If the Death Widget is in the Viewport, remove it and make it a nullptr
			if ( m_pcDeathWidgetInstance && m_pcDeathWidgetInstance->IsInViewport() )
			{
				m_pcDeathWidgetInstance->RemoveFromParent();
				m_pcDeathWidgetInstance = nullptr;
			}
		}
	}

	//check for how long melee is pressed
	if (m_bIsChargedAttackPressed)
	{
		m_fCurrentMeleePressedDuration += fDeltaTime;

		//this if condition is to start the shaking of camera after certain duration
		//better to remove this condition and call Shake camera from animations
		if (m_fCurrentMeleePressedDuration > m_fHoldDurationBeforeShaking)
		{
			ShakeCamera();
		}
	}

	//Set the counters to 1 here if the conditions are satisfied
	if (m_pcPlayerStatsComponent->GetPlayerCurrentHealth() <= m_fLowHealWarnLimit)
	{
		m_iHealthLowTimeCounter = 1;
	}

	if (m_iHealthLowTimeCounter)
	{
		if (m_pcPlayerStatsComponent->GetPlayerCurrentHealth() > m_fLowHealWarnLimit)
		{
			m_iHealthLowTimeCounter = 0;
		}
		if (m_pcLowHealthPPMat)
		{
			m_pcFollowCamera->AddOrUpdateBlendable(m_pcLowHealthPPMat, m_iHealthLowTimeCounter);
		}
	}

	//check if the player is invulnerable, if yes, reduce the delta time
	if (m_pcPlayerStatsComponent->IsPlayerInvulnerable())
	{
		m_pcPlayerStatsComponent->DecrementInvulnerabilityDuration(fDeltaTime);

		//check if the duration is less than 0, if yes set the player vulnerable
		if (m_pcPlayerStatsComponent->GetCurrentInvulnerabilityDuration() <= 0.0f)
		{
			m_pcPlayerStatsComponent->SetPlayerInvulnerability(false);
			m_pcPlayerStatsComponent->SetCurrentInvulnerabilityDuration(0.0f);
		}
	}
}

void AEchoesCharacter::SetControlRotation( float fP, float fY, float fR )
{
	if( !CameraLockCheck() )
	{
		Controller->SetControlRotation( FRotator( fP, fY, fR ) );
	}
}

void AEchoesCharacter::SetFOV( float fValue )
{
	if( !CameraLockCheck() )
	{
		m_pcFollowCamera->FieldOfView = FMath::Clamp( fValue, 1.0f, 179.0f );
	}
}

void AEchoesCharacter::SetArmDistance( float fValue )
{
	if( !CameraLockCheck() )
	{
		m_pcCameraBoom->TargetArmLength = FMath::Clamp( fValue, 0.0f, 3000.0f );
	}
}

void AEchoesCharacter::ChangeFOV( float fValue )
{
	if( !CameraLockCheck() )
	{
		float fUnclampedFOV = m_pcFollowCamera->FieldOfView + fValue;
		m_pcFollowCamera->FieldOfView = FMath::Clamp( fUnclampedFOV, 1.0f, 179.0f );
	}
}

void AEchoesCharacter::ChangeDistance( float fValue )
{
	if( !CameraLockCheck() )
	{
		const float kfMultiplier = 10.0f;
		float fUnclampedLength = m_pcCameraBoom->TargetArmLength + ( fValue * kfMultiplier );
		m_pcCameraBoom->TargetArmLength = FMath::Clamp( fUnclampedLength, 0.0f, 3000.0f );
	}
}

float AEchoesCharacter::GetSpringArmLength()
{
	return m_pcCameraBoom->TargetArmLength;
}

float AEchoesCharacter::GetCameraFOV()
{
	return m_pcFollowCamera->FieldOfView;
}

void AEchoesCharacter::LockCamera()
{
	m_bIsCameraLocked = !( m_bIsCameraLocked );
	if( GEngine )
	{
		FString sDebugMessage = ( ( m_bIsCameraLocked ) ? TEXT( "Camera is Locked!" ) : TEXT( "Camera is Unlocked!" ) );
		GEngine->AddOnScreenDebugMessage( -1, 15.0f, FColor::Red, sDebugMessage );
	}
}

void AEchoesCharacter::ShakeCamera()
{
	if( m_pcPlayerCameraShake )
	{
		float shakeScale = ( m_fCurrentMeleePressedDuration - m_fHoldDurationBeforeShaking ) * m_fShakeScaleMultiplier;
		if( shakeScale > m_fMaxShakeScale )
		{
			shakeScale = m_fMaxShakeScale;
		}

		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake( m_pcPlayerCameraShake, shakeScale );
	}
}

// to start detecting the time for which the melee attack was pressed
void  AEchoesCharacter::StartChargedAttackPressedDurationMeasuring()
{
	

	//Prevent input if the player is trapped
	if( !m_bIsPlayerTrapped )
	{ 
		// set to duration to 0 only when its not already pressed
		if (!m_bIsChargedAttackPressed)
		{
			m_fCurrentMeleePressedDuration = 0.0f;
		}
		m_bIsChargedAttackPressed = true;
	}
}

// to start detecting the time for which the fire projectile was pressed
void  AEchoesCharacter::StopChargedAttackPressedDurationMeasuring()
{
	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StopAllCameraShakes( true );
	
	m_bIsChargedAttackPressed = false;
	m_fCurrentMeleePressedDuration = 0.0f;

	//broadcast the delegate to stop all the particle effects on pick axe
	if ( m_dPlayerStopChargedDelegate.IsBound())
	{
		m_dPlayerStopChargedDelegate.Broadcast ( );
	}
}

void AEchoesCharacter::TurnAtRate( float fRate )
{
	if( !m_bIsCameraLocked )
	{
		// calculate delta for this frame from the rate information
		AddControllerYawInput( fRate * m_fBaseTurnRate * GetWorld()->GetDeltaSeconds() );
	}
}

void AEchoesCharacter::LookUpAtRate( float fRate )
{
	if( !m_bIsCameraLocked )
	{
		// calculate delta for this frame from the rate information
		AddControllerPitchInput( fRate * m_fBaseLookUpRate * GetWorld()->GetDeltaSeconds() );
	}
}

void AEchoesCharacter::MoveForward( float fValue )
{
	//Prevent input if the player is trapped or she is performing a charged attack
	if( !m_bIsPlayerTrapped && !m_bIsChargedAttackPressed )
	{
		// Limit movement while player is attacking or blocking
		// Added because current animation does not have attack/block and run blending
		if( ( Controller != nullptr ) && ( fValue != 0.0f ) )
		{
			// find out which way is forward
			const FRotator sRotation = Controller->GetControlRotation();
			const FRotator sYawRotation ( 0 , sRotation.Yaw , 0 );

			// get forward vector
			const FVector sDirection = FRotationMatrix( sYawRotation ).GetUnitAxis( EAxis::X );
			
			// allow movement only if player is not attacking,
			// or if she is attacking, but not the core attack is going on
			if (!m_bIsAttacking || !m_bIsNextAttackWindowOpen ) 
			{
				AddMovementInput( sDirection, fValue );
			}
		}
	}
}

void AEchoesCharacter::MoveRight( float fValue )
{
	//Prevent input if the player is trapped or she is performing a charged attack
	if( !m_bIsPlayerTrapped && !m_bIsChargedAttackPressed )
	{
		// Limit movement while player is attacking or blocking
		// Added because current animation does not have attack/block and run blending
		if( ( Controller != nullptr ) && ( fValue != 0.0f ) )
		{
			// find out which way is right
			const FRotator sRotation = Controller->GetControlRotation();
			const FRotator sYawRotation ( 0 , sRotation.Yaw , 0 );

			// get right vector 
			const FVector sDirection = FRotationMatrix( sYawRotation ).GetUnitAxis( EAxis::Y );

			// allow movement only if player is not attacking,
			// or if she is attacking, but not the core attack is going on
			if( !m_bIsAttacking|| !m_bIsNextAttackWindowOpen )
			{
				AddMovementInput( sDirection, fValue );
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Function that allows the damage to take damage and process it accordingly
//////////////////////////////////////////////////////////////////////////
float AEchoesCharacter::TakeDamage( float fDamageAmount, struct FDamageEvent const& rsDamageEvent,
	AController* pcEventInstigator, AActor* pcDamageCauser )
{
	//check the pointer of the damage causer is valid and if the player is vulnerable
	if( pcDamageCauser && !m_pcPlayerStatsComponent->IsPlayerInvulnerable() && m_pcPlayerStatsComponent->GetPlayerCurrentHealth() > 0.0f )
	{
		// The health is getting decreased by the damage amount the player takes
		m_pcPlayerStatsComponent->AddHealth( -1.0f * fDamageAmount );

		// Break the stun - stun should not persist
		//Clear the stun timer (regardless if set), and release the player from any stun effect
		GetWorld()->GetTimerManager().ClearTimer( m_sStunCBTimer );
		CBStunRelease();

		// Play vibration feedback
		FLatentActionInfo sLatentInfo;
		sLatentInfo.CallbackTarget = this;

		// A non null UObject needs to be provided as the callback target else this will not work
		// Stop the previous effect and start a new one
		GetWorld()->GetFirstPlayerController()->PlayDynamicForceFeedback
		(
			0.f, 0.f, false, false, false, false, EDynamicForceFeedbackAction::Stop, sLatentInfo
		);
		GetWorld()->GetFirstPlayerController()->PlayDynamicForceFeedback
		(
			1.f, 1.0f, true, true, true, true, EDynamicForceFeedbackAction::Start, sLatentInfo
		);

		// If the current health is less or equal to zero, the player should die
		if( m_pcPlayerStatsComponent->GetPlayerCurrentHealth() <= 0.0f )
		{
			
			UPlayerAnimInstance* pcAnimInstance = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
			if (pcAnimInstance)
			{
				pcAnimInstance->m_bIsDead = true;
			}

			// Play death sound
			if ( m_pcDeathSound )
			{
				UGameplayStatics::PlaySoundAtLocation ( this , m_pcDeathSound , GetActorLocation ( ) );
			}

			// Make it so that the Player has died and no input can be consumed
			DeathInput();
		}
		else
		{
			// If the player is not in the Corruption
			if ( !m_pcPlayerStatsComponent->GetCorruptionOverlapping() )
			{
				// the player is not dead, so make her invulnerable and set the vulnerable duration
				m_pcPlayerStatsComponent->SetCurrentInvulnerabilityDuration( m_pcPlayerStatsComponent->m_fInvulnerableDuration );
				m_pcPlayerStatsComponent->SetPlayerInvulnerability( true );
			}

			//play flinch sound
			if ( m_apcFlinchSound )
			{
				UGameplayStatics::PlaySoundAtLocation ( this , m_apcFlinchSound , GetActorLocation ( ) );
			}

			//broadcast the damage event using delegate
			if( m_dPlayerTakeDamageDelegate.IsBound() )
			{
				m_dPlayerTakeDamageDelegate.Broadcast( fDamageAmount );
			}
		}
	}

	return fDamageAmount;
}

//////////////////////////////////////////////////////////////////////////
// Call when the player has died
// Reset the player stats to the default values i.e. max health and energy
// Spawns the player back at the starting point of the level
//////////////////////////////////////////////////////////////////////////
void AEchoesCharacter::Reset()
{
	// If the action binding for Death Functionality has been assigned, remove it
	// The callback to Reset should ALWAYS be the last one to be added in this case
	if ( m_bRemoveDeathBinding )
	{
		m_pcMenuWidgetController->EnableInput( m_pcMenuWidgetController );

		m_bRemoveDeathBinding = false;
	}

	UPlayerAnimInstance* pcAnimInstance = Cast<UPlayerAnimInstance>( GetMesh()->GetAnimInstance() );
	if ( pcAnimInstance )
	{
		pcAnimInstance->m_bIsDead = false;
	}

	//Grapple/Stun reset - if active
	CBStunRelease();

	//Reset player stats
	m_pcPlayerStatsComponent->LiveStatReset();

	// Only restart the input on the pawn
	if ( m_pcMenuWidgetController )
	{
		m_pcMenuWidgetController->GetPawn()->EnableInput( m_pcMenuWidgetController );
	}

	if( m_sRespawnLocation == FVector::ZeroVector )
	{
		SetActorLocation( m_vStartingLocation );
	}
	else
	{
		SetActorLocation( m_sRespawnLocation );
	}
}

bool AEchoesCharacter::CameraLockCheck()
{
	if( m_bIsCameraLocked )
	{
		UE_LOG( LogTemp, Error, TEXT( "Camera is locked!" ) );
	}
	return m_bIsCameraLocked;
}

//////////////////////////////////////////////////////////////////////////
// Event method fired when the player lands from falling
//////////////////////////////////////////////////////////////////////////
void AEchoesCharacter::Landed( const FHitResult& krsHit )
{
	// Call the base method and reset the current jump value
	ACharacter::Landed( krsHit );
	m_iCurrentJump = 0;
	UPlayerAnimInstance* pcAnimInstance = Cast<UPlayerAnimInstance>( GetMesh()->GetAnimInstance() );
	if ( pcAnimInstance )
	{
		pcAnimInstance->m_bIsDoubleJumping = false;
	}
}

//////////////////////////////////////////////////////////////////////////
// Use to play the animation of the chest opening
//////////////////////////////////////////////////////////////////////////
void AEchoesCharacter::SetOpeningChestAnimState( bool bState )
{
	UPlayerAnimInstance* pcAnimInstance = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());

	if ( pcAnimInstance )
	{
		pcAnimInstance->m_bIsOpeningChest = bState;

		// If this Player Controller Exists
		if ( m_pcMenuWidgetController )
		{
			// If bState is true i.e. the chest is currently being opened
			if ( bState )
			{
				m_pcMenuWidgetController->GetPawn()->DisableInput( m_pcMenuWidgetController );
			}
			else
			{
				m_pcMenuWidgetController->GetPawn()->EnableInput( m_pcMenuWidgetController );
			}
		}
	}
}

void AEchoesCharacter::PlayFootPrintSound ( )
{
	if ( m_pcFootStepSound )
	{
		UGameplayStatics::PlaySoundAtLocation ( this , m_pcFootStepSound , GetActorLocation ( ) );
	}
}

//Called when boss death animation ends
void AEchoesCharacter::EndGame()
{
	if(m_bGameFinish) //Do not run multiple times
	{
		return;
	}

	if (m_pcLowHealthPPMat && m_iHealthLowTimeCounter)
	{
		m_pcFollowCamera->AddOrUpdateBlendable(m_pcLowHealthPPMat, 0.0f);
	}

	m_iStage = 1;
	m_bGameFinish = true;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Called when the player has been killed and the Death Widget is visible
//////////////////////////////////////////////////////////////////////////
void AEchoesCharacter::DeathInput()
{
	m_pcMenuWidgetController->DisableInput( m_pcMenuWidgetController );

	m_bRemoveDeathBinding = true;
	m_bIsPlayerTrapped = true;

	// If there is no instance already
	if ( !m_pcDeathWidgetInstance )
	{
		// And there is a valid widget
		if ( m_pcDeathWidget )
		{
			// Create and display the Death Popup
			m_pcDeathWidgetInstance = UWidgetBlueprintLibrary::Create( this, m_pcDeathWidget, GetWorld()->GetFirstPlayerController() );
			m_pcDeathWidgetInstance->AddToViewport( 20 );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Called to play the map update sound
//////////////////////////////////////////////////////////////////////////
void AEchoesCharacter::PlayMapUpdateSound ( )
{
	if ( m_pcMapUpdateSound )
	{
		UGameplayStatics::PlaySoundAtLocation ( this , m_pcMapUpdateSound , GetActorLocation ( ) );
	}
}

//////////////////////////////////////////////////////////////////////////
// Called to play the widget popup sound
//////////////////////////////////////////////////////////////////////////
void AEchoesCharacter::PlayWidgetPopupSound ( )
{
	if ( m_pcWidgetPopupSound )
	{
		UGameplayStatics::PlaySoundAtLocation ( this , m_pcWidgetPopupSound , GetActorLocation ( ) );
	}
}
