// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Orbit3DGameMode.h"
#include "Orbit3DPawn.h"

AOrbit3DGameMode::AOrbit3DGameMode()
{
	// set default pawn class to our flying pawn
	DefaultPawnClass = AOrbit3DPawn::StaticClass();

	if (GetWorld()) {
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AOrbit3DGameMode::SpawnSphere, 1.0f, true);
	}

	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AOrbit3DGameMode::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AOrbit3DGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;

	float iMass, jMass, DistCubed;
	FVector iPos, jPos;
	ASphereActor *iActor, *jActor;

	for (int i = 0, End = ActorArray.Num(); i < End - 2; i++) {
		iActor = ActorArray[i];
		iMass = iActor->GetMass();
		iPos = iActor->GetActorLocation();
		for (int j = i + 1; j < End - 1; j++) {
			jActor = ActorArray[j];
			// Apply force of gravity both ways
			// vel += (G * mass2 * (pos2 - pos1)) / (dist ^ 3)
			
			jMass = jActor->GetMass();
			jPos = jActor->GetActorLocation();

			DistCubed = FMath::Pow(FVector::Dist(iPos, jPos), 3.f);

			iActor->IncrementVelocity((G * jMass * (jPos - iPos)) / DistCubed);
			jActor->IncrementVelocity((G * iMass * (iPos - jPos)) / DistCubed);
		}
	}

}

void AOrbit3DGameMode::SpawnSphere() {
	UWorld* World = GetWorld();
	if (World) {
		FActorSpawnParameters SpawnParams;

		FVector SpawnLocation;
		SpawnLocation.X = -5000 + std::rand() % 10000; // -5000 to 5000
		SpawnLocation.Y = -5000 + std::rand() % 10000; // -5000 to 5000
		SpawnLocation.Z = std::rand() % 5000; // 0 to 5000


		ASphereActor *Sphere = World->SpawnActor<ASphereActor>(SpawnLocation, FRotator::ZeroRotator);
		Sphere->SetScale((std::rand() % 2) + 0.5f); // 0.5 to 2.5

		ActorArray.Add(Sphere);
	}

}




























