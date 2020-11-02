// Fill out your copyright notice in the Description page of Project Settings.


#include "GwangCar.h"
#include "DrawDebugHelpers.h"
#include "Net\UnrealNetwork.h"
#include "GwangCarMovementComponent.h"

// Sets default values
AGwangCar::AGwangCar()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	MovementComponent = CreateDefaultSubobject<UGwangCarMovementComponent>(TEXT("GwangCarMovement"));
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

void AGwangCar::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGwangCar, ServerState);
}

// Called every frame
void AGwangCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		FVehicleMove Move = MovementComponent->CreateMove(DeltaTime);
		MovementComponent->SimulateMove(Move);

		UnacknowledgedMoves.Add(Move);
		Server_SendMove(Move);
	}

	if (GetLocalRole() == ROLE_Authority && IsLocallyControlled())
	{
		FVehicleMove Move = MovementComponent->CreateMove(DeltaTime);
		Server_SendMove(Move);
	}

	if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		MovementComponent->SimulateMove(ServerState.LastMove);
	}

	DrawDebugString(GetWorld(), FVector(0, 0, 100), UEnum::GetValueAsString(GetLocalRole()), this, FColor::Cyan, DeltaTime);

}

// Called to bind functionality to input
void AGwangCar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGwangCar::Client_MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGwangCar::Client_MoveRight);
}

void AGwangCar::Client_MoveForward(float Value)
{
	MovementComponent->SetThrottle(Value);
}

void AGwangCar::Client_MoveRight(float Value)
{
	MovementComponent->SetSteeringThrow(Value);
}

void AGwangCar::ClearUnacknowledgedMoves(FVehicleMove LastMove)
{
	TArray<FVehicleMove> NewMoves;
	for (const FVehicleMove& Move : UnacknowledgedMoves)
	{
		if (Move.Time >= LastMove.Time)
		{
			NewMoves.Add(Move);
		}
	}

	UnacknowledgedMoves = NewMoves;
}

void AGwangCar::Server_SendMove_Implementation(FVehicleMove Move)
{
	MovementComponent->SimulateMove(Move);
	ServerState.LastMove = Move;
	ServerState.Transform = GetActorTransform();
	ServerState.Velocity = MovementComponent->GetVelocity();
}

bool AGwangCar::Server_SendMove_Validate(FVehicleMove Move)
{
	return true;
}

void AGwangCar::OnRep_ServerState()
{
	SetActorTransform(ServerState.Transform);
	MovementComponent->SetVelocity(ServerState.Velocity);

	ClearUnacknowledgedMoves(ServerState.LastMove);

	for (const FVehicleMove& Move : UnacknowledgedMoves)
	{
		MovementComponent->SimulateMove(Move);
	}
}