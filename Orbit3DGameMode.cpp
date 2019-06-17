#include "Orbit3DGameMode.h"
#include "Orbit3DPawn.h"
#include "Kismet/GameplayStatics.h"

#define MAX_SYSTEM_ORBIT_RADIUS 10000.f
#define MIN_SYSTEM_ORBIT_RADIUS 1000.f
#define MAX_PLANET_ORBIT_RADIUS 750.f
#define MIN_PLANET_ORBIT_RADIUS 400.f
#define MIN_PLANETS 5
#define MAX_PLANETS 15
#define MIN_MOONS 0
#define MAX_MOONS 3

/*
Creates and sets default values.
*/
AOrbit3DGameMode::AOrbit3DGameMode()
{
	// Set default pawn class to the flying pawn
	DefaultPawnClass = AOrbit3DPawn::StaticClass();

	// Start timer to spawn spheres in random locations
	if (GetWorld()) 
	{
		//GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AOrbit3DGameMode::SpawnSphere, 1.0f, true);
	}

	// Enable ticks every frame
	PrimaryActorTick.bCanEverTick = true;

	SpawnPlanetarySystem(FVector(0.f), FVector(0.f));
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
	for (int i = 0, End = ActorArray.Num(); i < End - 1; i++) 
	{
		// Make sure this actor is valid and active
		if (ActorArray[i] && ActorArray[i]->IsActive())
		{
			iActor = ActorArray[i];
			iMass = iActor->GetMass();
			iPos = iActor->GetActorLocation();
			for (int j = i + 1; j < End; j++)
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

					//if (i == 0)
					//{
					//	UE_LOG(LogTemp, Warning, TEXT("AOrbit3DGameMode::Tick iMass = %f, jMass = %f, iPos = %s, jPos = %s, iPos-jPos = %s, dist = %f, DistCubed = %f, G = %f"),
					//		iMass, jMass, *(iPos.ToString()), *(jPos.ToString()), *((iPos-jPos).ToString()), FVector::Dist(iPos, jPos), DistCubed, G);
					//}
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
	if (World) 
	{
		FActorSpawnParameters SpawnParams;

		FVector SpawnLocation;
		SpawnLocation.X = -5000 + std::rand() % 10000; // -5000 to 5000
		SpawnLocation.Y = -5000 + std::rand() % 10000; // -5000 to 5000
		SpawnLocation.Z = std::rand() % 5000; // 0 to 5000


		ASphereActor *Sphere = World->SpawnActor<ASphereActor>(SpawnLocation, FRotator::ZeroRotator);

		ActorArray.Add(Sphere);
	}
}

/*
Spawns NewSphere at a random distance away from Central, according to MaxOrbitDistance, 
then calculates the velocity required to stay in orbit. 
Vector calculations with help from:
https://math.stackexchange.com/questions/2347215/how-to-find-a-random-unit-vector-orthogonal-to-a-random-unit-vector-in-3d/2347293
*/
void AOrbit3DGameMode::CalculateOrbitVelocity(ASphereActor *NewSphere, ASphereActor *Central, float MinOrbitDistance, float MaxOrbitDistance)
{
	// Random unit vector * random acceptable radius to find random point inside sphere
	float Radius = FMath::FRandRange(MinOrbitDistance, MaxOrbitDistance);
	FVector Pos = Central->GetActorLocation() + FMath::VRand() * Radius;
	// Calculate required speed of the new ASphereActor
	// s = sqrt( (G * CentralMass) / radius of orbit )
	float Speed = FMath::Sqrt((G * Central->GetMass()) / Radius);
	// Take a simple perpendicular vector
	FVector Perp(Pos.Y, -Pos.X, 0);
	// Calculate cross product between Pos and Perp
	FVector Cross = FVector::CrossProduct(Pos, Perp);
	float Theta = FMath::FRandRange(0, 2 * PI);
	// Velocity direction vector - Random combination of Perp and Cross, still resulting in a normal vector
	FVector VelDir = FMath::Cos(Theta) * Perp.GetSafeNormal() + FMath::Sin(Theta) * Cross.GetSafeNormal();
	// Scale direction vector by scalar speed and add the central body's velocity
	FVector Vel = Central->GetVelocity() + Speed * VelDir.GetSafeNormal();

	UWorld *World = GetWorld();
	if (World)
	{
		NewSphere->SetVelocity(Vel);
		UE_LOG(LogTemp, Warning, TEXT("AOrbit3DGameMode::CalculateOrbitVelocity VelDir = %s, Vel = %s, Speed = %f, Central vel = %s, Radius = %f, CentralMass = %f, G = %f"),
			*(VelDir.ToString()), *(Vel.ToString()), Speed, *(Central->GetVelocity().ToString()), Radius, Central->GetMass(), G);
		UGameplayStatics::FinishSpawningActor(NewSphere, FTransform(Pos));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AOrbit3DGameMode::CalculateOrbitVelocity Failed to spawn sphere"));
	}
}

/*
Spawns a planetary system with the centre being the given Central ASphereActor. A random number 
of planets are spawned in orbit of the given central sphere, with a random number of asteroids 
orbiting those planets.
*/
void AOrbit3DGameMode::SpawnPlanetarySystem(ASphereActor *Central)
{
	TArray<ASphereActor*> NewPlanets, NewMoons;
	ASphereActor *NewPlanet, *NewMoon;

	// Generate between 2 and 5 planets around the central sphere
	int NumPlanets = MIN_PLANETS + FMath::RandHelper(MAX_PLANETS - MIN_PLANETS);
	int NumMoons = 0;
	for (int i = 0; i < NumPlanets; i++) 
	{
		NewPlanet = Cast<ASphereActor>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ASphereActor::StaticClass(), FTransform()));
		NewPlanet->SetType(ASphereActor::Planet);
		CalculateOrbitVelocity(NewPlanet, Central, MIN_SYSTEM_ORBIT_RADIUS, MAX_SYSTEM_ORBIT_RADIUS);
		NewPlanets.Add(NewPlanet);
		// Generate random number of moons to spawn around the planet
		NumMoons = MIN_MOONS + FMath::RandHelper(MAX_MOONS - MIN_MOONS);
		for (int j = 0; j < NumMoons; j++)
		{
			NewMoon = Cast<ASphereActor>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ASphereActor::StaticClass(), FTransform()));
			NewMoon->SetType(ASphereActor::Moon);
			CalculateOrbitVelocity(NewMoon, NewPlanet, MIN_PLANET_ORBIT_RADIUS, MAX_PLANET_ORBIT_RADIUS);
			NewMoons.Add(NewMoon);
		}
	}

	ActorArray.Add(Central);
	ActorArray.Append(NewPlanets);
	ActorArray.Append(NewMoons);
}

/*
Spawns a planetary system with the centre at the given position Pos and velocity Vel. A random 
number of planets are then spawned in orbit around this body, with a random number of asteroids 
orbiting those planets.
*/
void AOrbit3DGameMode::SpawnPlanetarySystem(FVector Pos, FVector Vel)
{
	UWorld *World = GetWorld();
	if (World)
	{
		// Deferred spawn - create the object, but don't spawn it until we've initialised it
		ASphereActor *Sphere = Cast<ASphereActor>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ASphereActor::StaticClass(), FTransform()));
		//ASphereActor *Sphere = World->SpawnActor<ASphereActor>(Pos, FRotator::ZeroRotator);

		if (Sphere != nullptr)
		{
			// Init values
			Sphere->SetVelocity(Vel);
			Sphere->SetType(ASphereActor::Sun);
			UWorld *World = GetWorld();

			// Finish spawning
			UGameplayStatics::FinishSpawningActor(Sphere, FTransform(Pos));

			SpawnPlanetarySystem(Sphere);
		}
	}
}


























