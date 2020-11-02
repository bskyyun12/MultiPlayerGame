// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "GwangCarMovementComponent.h"

#include "GwangCar.generated.h"

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

UCLASS()
class MULTIPLAYGAME_API AGwangCar : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGwangCar();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UGwangCarMovementComponent* GetVehicleMovementComponent() { return MovementComponent; }

private:
	
	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FVehicleState ServerState;

	UFUNCTION()
	void OnRep_ServerState();

	TArray<FVehicleMove> UnacknowledgedMoves;

private:
	
	void ClearUnacknowledgedMoves(FVehicleMove LastMove);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FVehicleMove Move);
	void Server_SendMove_Implementation(FVehicleMove Move);
	bool Server_SendMove_Validate(FVehicleMove Move);

	void Client_MoveForward(float Value);
	void Client_MoveRight(float Value);

	UPROPERTY(EditAnywhere)
	UGwangCarMovementComponent* MovementComponent;
};
