// Fill out your copyright notice in the Description page of Project Settings.


#include "Skybox.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

// Sets default values
ASkybox::ASkybox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Base = CreateDefaultSubobject<USphereComponent>(TEXT("Base"));
	Base->bHiddenInGame = true;
	Base->Mobility = EComponentMobility::Movable;
	RootComponent = Base;

	// Set mesh to sphere
	SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereMesh"));
	SphereMesh->AttachTo(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	if (SphereMeshAsset.Succeeded()) {
		SphereMesh->SetStaticMesh(SphereMeshAsset.Object);
		SphereMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		SphereMesh->SetWorldScale3D(FVector(1000.0f));
	}
	// Set texture to SpaceMaterial
	Material = CreateDefaultSubobject<UMaterial>(TEXT("Material"));
	// Set timer, ticking every 10 sec
	// Call function that moves mesh to location of player

	// Disable collisions
	SetActorEnableCollision(false);

}

// Called when the game starts or when spawned
void ASkybox::BeginPlay()
{
	Super::BeginPlay();

	SphereMesh->SetMaterial(0, Material);
	if (GetWorld()) {
		GetWorldTimerManager().SetTimer(MoveTimerHandle, this, &ASkybox::MoveToPlayer, 1.0f, true);
	}
	
}

// Called every frame
void ASkybox::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);

}

void ASkybox::MoveToPlayer() {
	// get first player pawn location
	if (GetWorld() && GetWorld()->GetFirstPlayerController()) {
		FVector PawnLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
		//UE_LOG(LogTemp, Warning, TEXT("GetWorld"));

		// screen log player location
		if (GEngine)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Player Location: %s"), *PawnLocation.ToString()));
		}
		// Sphere's location is the centre bottom
		PawnLocation.Z -= 10000.0f; // Ideally would minus the radius of the sphere
		SetActorLocation(PawnLocation);
	}
}




