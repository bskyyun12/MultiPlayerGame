// Fill out your copyright notice in the Description page of Project Settings.


#include "GwangCar.h"
#include "DrawDebugHelpers.h"
#include "GwangCarMovementComponent.h"

// Sets default values
AGwangCar::AGwangCar()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicatingMovement(false);
	SetReplicates(true);

	MovementComponent = CreateDefaultSubobject<UGwangCarMovementComponent>(TEXT("GwangCarMovement"));
	MoveReplicationComponent = CreateDefaultSubobject<UGwangCarMoveReplicationComponent>(TEXT("GwangCarMovementReplication"));
}

// Called when the game starts or when spawned
void AGwangCar::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		NetUpdateFrequency = 1;
	}

	if (!ensure(MovementComponent != nullptr))
	{
		return;
	}
}

//void AGwangCar::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//	DOREPLIFETIME(AGwangCar, ServerState);
//}

// Called every frame
void AGwangCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DrawDebugString(GetWorld(), FVector(0, 0, 100), UEnum::GetValueAsString(GetLocalRole()), this, FColor::Cyan, DeltaTime);
}

// Called to bind functionality to input
void AGwangCar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGwangCar::Client_MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGwangCar::Client_MoveRight);

	PlayerInputComponent->BindAction("Drift", IE_Pressed, this, &AGwangCar::DriftPressed);
	PlayerInputComponent->BindAction("Drift", IE_Released, this, &AGwangCar::DriftReleased);
}

void AGwangCar::Client_MoveForward(float Value)
{
	MovementComponent->SetThrottle(Value);
}

void AGwangCar::Client_MoveRight(float Value)
{
	MovementComponent->SetSteeringThrow(Value);
}

void AGwangCar::DriftPressed()
{
	OnDrift.Broadcast();
	MovementComponent->SetDrift(true);
}

void AGwangCar::DriftReleased()
{
	OnStopDrift.Broadcast();
	MovementComponent->SetDrift(false);
}