// Fill out your copyright notice in the Description page of Project Settings.


#include "GwangCarMoveReplicationComponent.h"
#include "Net\UnrealNetwork.h"
#include "GwangCarMovementComponent.h"

// Sets default values for this component's properties
UGwangCarMoveReplicationComponent::UGwangCarMoveReplicationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

// Called when the game starts
void UGwangCarMoveReplicationComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
	MovementComponent = GetOwner()->FindComponentByClass<UGwangCarMovementComponent>();
}

void UGwangCarMoveReplicationComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UGwangCarMoveReplicationComponent, ServerState);
}

// Called every frame
void UGwangCarMoveReplicationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		FVehicleMove Move = MovementComponent->CreateMove(DeltaTime);
		MovementComponent->SimulateMove(Move);

		UnacknowledgedMoves.Add(Move);
		Server_SendMove(Move);
	}

	if (GetOwnerRole() == ROLE_Authority && GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy)
	{
		FVehicleMove Move = MovementComponent->CreateMove(DeltaTime);
		Server_SendMove(Move);
	}

	if (GetOwnerRole() == ROLE_SimulatedProxy)
	{
		MovementComponent->SimulateMove(ServerState.LastMove);
	}
}

void UGwangCarMoveReplicationComponent::ClearUnacknowledgedMoves(FVehicleMove LastMove)
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

void UGwangCarMoveReplicationComponent::Server_SendMove_Implementation(FVehicleMove Move)
{
	MovementComponent->SimulateMove(Move);
	ServerState.LastMove = Move;
	ServerState.Transform = GetOwner()->GetActorTransform();
	ServerState.Velocity = MovementComponent->GetVelocity();
}

bool UGwangCarMoveReplicationComponent::Server_SendMove_Validate(FVehicleMove Move)
{
	return true;
}

void UGwangCarMoveReplicationComponent::OnRep_ServerState()
{
	GetOwner()->SetActorTransform(ServerState.Transform);
	if (MovementComponent == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("MovementComponent is nullptr"));
		return;
	}
	MovementComponent->SetVelocity(ServerState.Velocity);

	ClearUnacknowledgedMoves(ServerState.LastMove);

	for (const FVehicleMove& Move : UnacknowledgedMoves)
	{
		MovementComponent->SimulateMove(Move);
	}
}