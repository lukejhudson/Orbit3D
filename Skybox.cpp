#include "Skybox.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

/*
Creates and sets default values. 
*/
ASkybox::ASkybox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Set mesh to sphere
	SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereMesh"));
	// Set mesh as root component for the actor
	RootComponent = SphereMesh;
	// Get the default sphere mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	if (SphereMeshAsset.Succeeded()) 
	{
		SphereMesh->SetStaticMesh(SphereMeshAsset.Object);
		SphereMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		// Make sphere very large
		SphereMesh->SetWorldScale3D(FVector(1000.0f));
	}
	// Create default texture
	Material = CreateDefaultSubobject<UMaterial>(TEXT("Material"));

	// Disable collisions
	SetActorEnableCollision(false);
	// Disable shadows
	//SphereMesh->bCastDynamicShadow = false;
}

/*
Called when the game starts or when spawned.
Updates the texture of the mesh and starts a timer to keep the sky box centred on the player
(making the spacescape background appear infinite).
*/
void ASkybox::BeginPlay()
{
	Super::BeginPlay();

	// Update texture to texture specified in editor
	SphereMesh->SetMaterial(0, Material);

	// Set timer, ticking every 10 sec
	// Call function that moves mesh to location of player
	if (GetWorld()) 
	{
		GetWorldTimerManager().SetTimer(MoveTimerHandle, this, &ASkybox::MoveToPlayer, 1.0f, true);
	}
	
}

// Called every frame if PrimaryActorTick.bCanEverTick is set to true (but currently false)
void ASkybox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/*
Moves the sky box to be roughly centred on the player of the game. 
Called by a timer every few seconds.
*/
void ASkybox::MoveToPlayer() 
{
	if (GetWorld() && GetWorld()->GetFirstPlayerController()) 
	{
		// Get first player pawn location
		FVector PawnLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
		//UE_LOG(LogTemp, Warning, TEXT("GetWorld"));

		// Screen log for debugging
		if (GEngine)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Player Location: %s"), *PawnLocation.ToString()));
		}
		// Sphere's location is the centre bottom
		PawnLocation.Z -= 10000.0f; // Ideally would minus the radius of the sphere
		SetActorLocation(PawnLocation);
	}
}




