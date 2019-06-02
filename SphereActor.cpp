#include "SphereActor.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"
#include "Orbit3DPawn.h"

#define DEFAULT_SCALE 0.8f
#define DEFAULT_MASS 100.f

// Sets default values
ASphereActor::ASphereActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set mesh to sphere
	SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereMesh"));
	// Make the mesh the root component of the actor
	RootComponent = SphereMesh;

	Scale = (std::rand() % 20) / 10.f + 0.5f; // 0.5 to 2.5

	// Get the default sphere mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	if (SphereMeshAsset.Succeeded()) {
		SphereMesh->SetStaticMesh(SphereMeshAsset.Object);
		SphereMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		SphereMesh->SetWorldScale3D(FVector(Scale));

		SphereMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		SphereMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		// When something collides with the mesh, call the OnHit function
		SphereMesh->OnComponentHit.AddDynamic(this, &ASphereActor::OnHit);
	}

	Velocity = FVector(0.f);
	Mass = DEFAULT_MASS * Scale * Scale;
}

// Called when the game starts or when spawned
void ASphereActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASphereActor::Tick(float DeltaTime)
{
	// Update the actor's location based on the current velocity
	// (with sweep so we stop when we collide with things and trigger OnHit events)
	AddActorLocalOffset(Velocity, true);

	Super::Tick(DeltaTime);
}

// Called when this actor collides with another
void ASphereActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("SphereActor OnHit"));
	// Find what we have collided with
	if (OtherActor->IsA(ASphereActor::StaticClass())) 
	{
		// Collided with another SphereActor
		ASphereActor *OtherSphere = Cast<ASphereActor>(OtherActor);
		// Return if other actor is inactive and thus marked for deletion
		if (!OtherSphere->IsActive()) return;
		// Compare masses
		float OtherMass = OtherSphere->GetMass();
		UE_LOG(LogTemp, Warning, TEXT("SphereActor OnHit Before, Mass = %f, OtherMass = %f, Scale = %f, OtherScale = %f, Velocity = %s, OtherVelocity = %s"),
			Mass, OtherMass, Scale, OtherSphere->GetScale(), *(Velocity.ToString()), *(OtherSphere->GetVelocity().ToString()));
		if (Mass > OtherMass) 
		{
			// Larger --> Consume; increase in size proportional to other SphereActor
			SetScale(Scale + (OtherMass / Mass));
			SetScale(Scale * (1 + (0.5 * OtherMass / Scale) / (Mass / Scale)));
			Mass += OtherMass;
			Velocity = (Mass * Velocity + OtherMass * OtherSphere->GetVelocity()) / (Mass + OtherMass);
			UE_LOG(LogTemp, Warning, TEXT("SphereActor OnHit After, Mass = %f, OtherMass = %f, Scale = %f, OtherScale = %f, Velocity = %s, OtherVelocity = %s"), 
				Mass, OtherMass, Scale, OtherSphere->GetScale(), *(Velocity.ToString()), *(OtherSphere->GetVelocity().ToString()));
		}
		else
		{
			// Smaller --> Mark self as inactive so game mode can destroy safely
			Active = false;
		}				
		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("I Hit ASphereActor: %s"), *OtherActor->GetName()));
	}
	else if (OtherActor->IsA(AOrbit3DPawn::StaticClass()))
	{
		// Collided with player (AOrbit3DPawn)
			// Game over
		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("I Hit AOrbit3DPawn: %s"), *OtherActor->GetName()));
	}
}

float ASphereActor::GetScale() {
	return Scale;
}

void ASphereActor::SetScale(float Scale) {
	SphereMesh->SetWorldScale3D(FVector(Scale));
	this->Scale = Scale;
}

FVector ASphereActor::GetVelocity() {
	return Velocity;
}

void ASphereActor::SetVelocity(FVector Vec) {
	Velocity = Vec;
}

void ASphereActor::IncrementVelocity(FVector Vec) {
	Velocity += Vec;
}

float ASphereActor::GetMass() {
	return Mass;
}

void ASphereActor::SetMass(float M) {
	Mass = M;
}

bool ASphereActor::IsActive()
{
	return Active;
}
