// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"

// Sets default values
AMovingPlatform::AMovingPlatform()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	Mesh->AttachTo(RootComponent);
}

// Called when the game starts or when spawned
void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();

	if (HasAuthority())
	{
		SetReplicates(true);
		SetReplicateMovement(true);
	}
}

// Called every frame
void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentNumOfTriggers >= RequiredNumOfTriggers)
	{
		if (HasAuthority())
		{
			Time += DeltaTime;
			// Smooth PingPong
			float Alpha = ((-FMath::Cos(PI * Time / TravalTime) + 1) / 2);
			FVector NewLocation = FMath::Lerp(StartLocation, StartLocation + TargetLocation, Alpha);
			SetActorLocation(NewLocation);
		}
	}
}

void AMovingPlatform::OnTriggerActivated()
{
	CurrentNumOfTriggers++;
}

void AMovingPlatform::OnTriggerDeactivated()
{
	CurrentNumOfTriggers = FMath::Max(0, --CurrentNumOfTriggers);
}

