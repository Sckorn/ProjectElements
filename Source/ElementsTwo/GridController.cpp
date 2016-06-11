// Fill out your copyright notice in the Description page of Project Settings.

#include "ElementsTwo.h"
#include "GridController.h"
#include "ElementsTwoGameMode.h"


// Sets default values
AGridController::AGridController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void AGridController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGridController::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	AElementsTwoGameMode * GameMode = Cast<AElementsTwoGameMode>(UGameplayStatics::GetGameMode(this));
}

