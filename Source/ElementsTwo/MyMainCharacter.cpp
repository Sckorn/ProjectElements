// Fill out your copyright notice in the Description page of Project Settings.

#include "ElementsTwo.h"
#include "MyMainCharacter.h"


// Sets default values
AMyMainCharacter::AMyMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	/*OurMainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	OurMainCamera->AttachParent = GetCapsuleComponent();
	OurMainCamera->SetRelativeLocation(FVector(500.0f, 500.0f, 64.0f));
	APlayerController * OurPlayerController = UGameplayStatics::GetPlayerController(this, 0);*/
	
	//OurPlayerController->SetViewTarget((AActor *)OurMainCamera);
}

// Called when the game starts or when spawned
void AMyMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyMainCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void AMyMainCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

