// Fill out your copyright notice in the Description page of Project Settings.


#include "SphereActor.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

// Sets default values
ASphereActor::ASphereActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	box = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("MyBox"));
	box->bHiddenInGame = true;
	box->Mobility = EComponentMobility::Movable;
	RootComponent = box;

	SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereMesh"));

	SphereMesh->AttachTo(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	if (SphereMeshAsset.Succeeded()) {
		SphereMesh->SetStaticMesh(SphereMeshAsset.Object);
		SphereMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		SphereMesh->SetWorldScale3D(FVector(0.8f));
	}

	Velocity = FVector(0.f);
	Mass = 100.f;
}

// Called when the game starts or when spawned
void ASphereActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASphereActor::Tick(float DeltaTime)
{
	SetActorLocation(GetActorLocation() + Velocity);

	Super::Tick(DeltaTime);
}

void ASphereActor::SetScale(float Scale) {
	SphereMesh->SetWorldScale3D(FVector(Scale));
	Mass *= Scale * Scale;
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