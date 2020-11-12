// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"
#include "Net\UnrealNetwork.h"
#include "GwangCarMovementComponent.h"

// Sets default values
AMovingPlatform::AMovingPlatform()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	Mesh->AttachTo(RootComponent);

	SetReplicates(true);
	SetReplicatingMovement(false);
}

// Called when the game starts or when spawned
void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
	TargetLocation = StartLocation + TargetLocation;

	if (HasAuthority())
	{
		NetUpdateFrequency = 1;	// update frequency from server
	}
}

void AMovingPlatform::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMovingPlatform, Server);
}

// Called every frame
void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ClientTime += (DeltaTime / TravalTime * (Hit.IsValidBlockingHit() ? -1 : 1));
	SimulateMove(ClientTime);

	if (HasAuthority())
	{
		// Update Server Variables
		Server.ReplicatedTime = ClientTime;
	}
}

void AMovingPlatform::OnRep_Server()
{
	ClientTime = Server.ReplicatedTime;
}

void AMovingPlatform::SimulateMove(float Time)
{
	// Smooth Infinite
	float Alpha = ((-FMath::Cos(PI * Time) + 1) / 2);
	FVector NewLocation = FMath::LerpStable(StartLocation, TargetLocation, Alpha);
	SetActorLocation(NewLocation, true, &Hit);
	HandleCollision();
}

void AMovingPlatform::HandleCollision()
{
	if (Hit.IsValidBlockingHit())
	{
		UGwangCarMovementComponent* MovementComponent = Hit.GetActor()->FindComponentByClass<UGwangCarMovementComponent>();
		if (MovementComponent != nullptr)
		{
			FVector PushDir = Hit.ImpactNormal;
			PushDir.Z = 0;
			MovementComponent->AddVelocity(PushDir * PushingPower);
		}
	}
}

