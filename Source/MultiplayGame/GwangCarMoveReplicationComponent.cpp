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

	FVehicleMove LastMove = MovementComponent->GetLastMove();

	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		UnacknowledgedMoves.Add(LastMove);
		Server_SendMove(LastMove);
	}

	if (GetOwnerRole() == ROLE_Authority && GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy)
	{
		UpdateServerState(LastMove);
	}

	if (GetOwnerRole() == ROLE_SimulatedProxy)
	{
		ClientTick(DeltaTime);
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
	if (MovementComponent == nullptr)
	{
		return;
	}
	MovementComponent->SimulateMove(Move);
	UpdateServerState(Move);
}

bool UGwangCarMoveReplicationComponent::Server_SendMove_Validate(FVehicleMove Move)
{
	return true;
}

void UGwangCarMoveReplicationComponent::UpdateServerState(const FVehicleMove& Move)
{
	ServerState.LastMove = Move;
	ServerState.Transform = GetOwner()->GetActorTransform();
	ServerState.Velocity = MovementComponent->GetVelocity();
}

void UGwangCarMoveReplicationComponent::ClientTick(float DeltaTime)
{
	//MovementComponent->SimulateMove(ServerState.LastMove);
	ClientTimeSinceUpdate += DeltaTime;

	if (ClientTimeBetweenLastUpdates < KINDA_SMALL_NUMBER)
	{
		return;
	}

	if (MovementComponent == nullptr)
	{
		return;
	}

	
	// Slope = Derivative = DeltaLocation / DeltaAlpha
	// Velocity = DeltaLocation / DeltaTime
	// DeltaAlpha = DeltaTime / TimeBetweenLastUpdates
	// Slope = Derivative = Velocity * TimeBetweenLastUpdates;

	float Alpha = ClientTimeSinceUpdate / ClientTimeBetweenLastUpdates;
	
	FVector StartLocation = ClientStartTransform.GetLocation();
	FVector TargetLocation = ServerState.Transform.GetLocation();

	float VelocityToDerivative = ClientTimeBetweenLastUpdates * 100; // x100 to convert meter to centimeter
	FVector StartDerivative = ClientStartVelocity * VelocityToDerivative;
	FVector TargetDerivative = ServerState.Velocity * VelocityToDerivative;

	FVector NewLocation = FMath::CubicInterp(StartLocation, StartDerivative, TargetLocation, TargetDerivative, Alpha);
	GetOwner()->SetActorLocation(NewLocation);

	FVector NewDerivative = FMath::CubicInterpDerivative(StartLocation, StartDerivative, TargetLocation, TargetDerivative, Alpha);
	FVector NewVelocity = NewDerivative / VelocityToDerivative;
	MovementComponent->SetVelocity(NewVelocity);

	FQuat StartRotation = ClientStartTransform.GetRotation();
	FQuat TargetRotation = ServerState.Transform.GetRotation();
	FQuat NewRotation = FQuat::Slerp(StartRotation, TargetRotation, Alpha);
	GetOwner()->SetActorRotation(NewRotation);
}

void UGwangCarMoveReplicationComponent::OnRep_ServerState()
{
	switch (GetOwnerRole())
	{
	case ROLE_AutonomousProxy:
		AutonomousProxy_OnRep_ServerState();
		break;
	case ROLE_SimulatedProxy:
		SimulatedProxy_OnRep_ServerState();
		break;
	default:
		break;
	}
}

void UGwangCarMoveReplicationComponent::AutonomousProxy_OnRep_ServerState()
{
	if (MovementComponent == nullptr)
	{
		return;
	}
	GetOwner()->SetActorTransform(ServerState.Transform);
	MovementComponent->SetVelocity(ServerState.Velocity);

	ClearUnacknowledgedMoves(ServerState.LastMove);

	for (const FVehicleMove& Move : UnacknowledgedMoves)
	{
		MovementComponent->SimulateMove(Move);
	}
}

void UGwangCarMoveReplicationComponent::SimulatedProxy_OnRep_ServerState()
{
	if (MovementComponent == nullptr)
	{
		return;
	}
	ClientTimeBetweenLastUpdates = ClientTimeSinceUpdate;
	ClientTimeSinceUpdate = 0;
	ClientStartTransform = GetOwner()->GetActorTransform();
	ClientStartVelocity = MovementComponent->GetVelocity();
}
