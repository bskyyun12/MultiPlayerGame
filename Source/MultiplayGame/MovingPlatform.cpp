// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"
#include "Net\UnrealNetwork.h"

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
	DOREPLIFETIME(AMovingPlatform, Time);
	DOREPLIFETIME(AMovingPlatform, ReplicatedTranform);
}

// Called every frame
void AMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentNumOfTriggers >= RequiredNumOfTriggers)
	{
		#pragma region SmoothStep
		// SmmothStep
		//if (FVector::Dist(GetActorLocation(), TargetLocation) < 0.01)
		//{
		//	Time = 0;
		//	FVector TempStartPos = StartLocation;
		//	StartLocation = TargetLocation;
		//	TargetLocation = TempStartPos;
		//}
		//Time += DeltaTime / TravalTime;
		//float Alpha = Time * Time * Time * (Time * (Time * 6 - 15) + 10);
		//FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, Alpha);
		//SetActorLocation(NewLocation);
#pragma endregion

		if (HasAuthority())
		{
			ReplicatedTranform = GetActorTransform(); // Save Server's transform
		}

		// Smooth Infinite
		Time += DeltaTime / TravalTime;
		float Alpha = ((-FMath::Cos(PI * Time) + 1) / 2);
		FVector NewLocation = FMath::Lerp(StartLocation, TargetLocation, Alpha);
		SetActorLocation(NewLocation);
	}
}

void AMovingPlatform::OnRep_ReplicatedTranform()
{
	SetActorTransform(ReplicatedTranform);	// Set server's transform into clients' transform
}

void AMovingPlatform::OnTriggerActivated()
{
	CurrentNumOfTriggers++;
}

void AMovingPlatform::OnTriggerDeactivated()
{
	CurrentNumOfTriggers = FMath::Max(0, --CurrentNumOfTriggers);
}

