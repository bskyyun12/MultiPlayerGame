// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnTrigger.h"

#include "Components/BoxComponent.h"

#include "PlayerInterface.h"
#include "MovingPlatform.h"

// Sets default values
ASpawnTrigger::ASpawnTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	if (!ensure(TriggerVolume != nullptr))
	{
		return;
	}
	RootComponent = TriggerVolume;
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ASpawnTrigger::BeginOverlap);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &ASpawnTrigger::EndOverlap);
}

// Called when the game starts or when spawned
void ASpawnTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpawnTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpawnTrigger::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->GetClass()->ImplementsInterface(UPlayerInterface::StaticClass()))
	{
		for (AMovingPlatform* Platform : PlatformsToTrigger)
		{
			Platform->OnTriggerActivated();
		}
	}
}

void ASpawnTrigger::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->GetClass()->ImplementsInterface(UPlayerInterface::StaticClass()))
	{
		for (AMovingPlatform* Platform : PlatformsToTrigger)
		{
			Platform->OnTriggerDeactivated();
		}
	}
}

