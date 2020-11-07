// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GwangCarMovementComponent.h"
#include "GwangCarMoveReplicationComponent.generated.h"

USTRUCT()
struct FVehicleState
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	FVector Velocity;

	UPROPERTY()
	FVehicleMove LastMove;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYGAME_API UGwangCarMoveReplicationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGwangCarMoveReplicationComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


private:

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FVehicleState ServerState;

	UFUNCTION()
	void OnRep_ServerState();
	void AutonomousProxy_OnRep_ServerState();
	void SimulatedProxy_OnRep_ServerState();

	TArray<FVehicleMove> UnacknowledgedMoves;
	void ClearUnacknowledgedMoves(FVehicleMove LastMove);

	void UpdateServerState(const FVehicleMove& Move);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FVehicleMove Move);
	void Server_SendMove_Implementation(FVehicleMove Move);
	bool Server_SendMove_Validate(FVehicleMove Move);

	float ClientTimeSinceUpdate;
	float ClientTimeBetweenLastUpdates;
	FTransform ClientStartTransform;
	FVector ClientStartVelocity;

	void ClientTick(float DeltaTime);

	UPROPERTY()
	UGwangCarMovementComponent* MovementComponent;

	UPROPERTY()
	USceneComponent* MeshOffsetRoot;

	UFUNCTION(BlueprintCallable)
	void SetMeshOffsetRoot(USceneComponent* Root) { MeshOffsetRoot = Root; }
};
