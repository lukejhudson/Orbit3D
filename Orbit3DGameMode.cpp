#include "Orbit3DGameMode.h"
#include "Orbit3DPawn.h"

/*
Creates and sets default values.
*/
AOrbit3DGameMode::AOrbit3DGameMode()
{
	// Set default pawn class to the flying pawn
	DefaultPawnClass = AOrbit3DPawn::StaticClass();

	// Start timer to spawn spheres in random locations
	if (GetWorld()) {
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AOrbit3DGameMode::SpawnSphere, 1.0f, true);
	}

	// Enable ticks every frame
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AOrbit3DGameMode::BeginPlay()
{
	Super::BeginPlay();
}

/*
Called every frame. Performs gravity calculations between all members of ActorArray 
and updates their velocities accordingly.
*/
void AOrbit3DGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;

	float iMass, jMass, DistCubed;
	FVector iPos, jPos;
	ASphereActor *iActor, *jActor;

	// When an actor is removed, all references are set to nullptr and its memory
	// is cleaned up by the garbage collector at a later time
	// -> Should be able to check for nullptrs and if so mark them for removal
	// from the ActorArray list after the loop is completed. 
	// (Could also consider GetAllActorsOfClass(...))
	for (int i = 0, End = ActorArray.Num(); i < End - 2; i++) 
	{
		// Make sure this actor is valid and active
		if (ActorArray[i] && ActorArray[i]->IsActive())
		{
			iActor = ActorArray[i];
			iMass = iActor->GetMass();
			iPos = iActor->GetActorLocation();
			for (int j = i + 1; j < End - 1; j++)
			{
				// Ensure other actor is valid and active
				if (ActorArray[j] && ActorArray[j]->IsActive())
				{
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
	}
	for (int i = ActorArray.Num() - 1; i >= 0; i--) 
	{
		// Remove if nullptr or inactive
		if (!ActorArray[i] || !ActorArray[i]->IsActive()) 
		{
			ActorArray[i]->Destroy();
			ActorArray.RemoveAt(i);
		}
	}
}

/*
Spawns a single sphere (ASphereActor) in a (roughly) random location, and adds a 
pointer to the actor to the TArray ActorArray.
*/
void AOrbit3DGameMode::SpawnSphere() {
	UWorld* World = GetWorld();
	if (World) {
		FActorSpawnParameters SpawnParams;

		FVector SpawnLocation;
		SpawnLocation.X = -5000 + std::rand() % 10000; // -5000 to 5000
		SpawnLocation.Y = -5000 + std::rand() % 10000; // -5000 to 5000
		SpawnLocation.Z = std::rand() % 5000; // 0 to 5000


		ASphereActor *Sphere = World->SpawnActor<ASphereActor>(SpawnLocation, FRotator::ZeroRotator);

		ActorArray.Add(Sphere);
	}

}




























