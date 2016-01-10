// Fill out your copyright notice in the Description page of Project Settings.

#include "ElementsTwo.h"
#include "ElementsTwoGameMode.h"
#include "BaseElementCube.h"


// Sets default values
ABaseElementCube::ABaseElementCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VisibleCompponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DefaultCube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshComponent(TEXT("/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube"));
	//static ConstructorHelpers::FObjectFinder<UMaterial> MyMaterial(TEXT("Material'/Game/StarterContent/Materials/M_Basic_Ice_Cosine.M_Basic_Ice_Cosine'"));
	if (MeshComponent.Succeeded())
	{
		VisibleCompponent->SetStaticMesh(MeshComponent.Object);
		/*if (MyMaterial.Succeeded())
		{
			VisibleCompponent->SetMaterial(0, MyMaterial.Object);
		}*/
	}
	VisibleCompponent->AttachTo(RootComponent);
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("OverlappingSphere"));
	SphereComponent->InitSphereRadius(200.0f);
	SphereComponent->AttachTo(VisibleCompponent);
	VisibleCompponent->OnClicked.AddDynamic(this, &ABaseElementCube::OnClicked);
	differenceBetweenCenters = 110.0f;
	bAfterDestroyGenerated = false;
	scoreValue = 10;
	bDestroyed = false;
	iSpecialActionModifier = 1;
}

// Called when the game starts or when spawned
void ABaseElementCube::BeginPlay()
{
	Super::BeginPlay();
	InitialPosition = GetActorLocation();
	alpha = 0;
	direction = 1;
	UpLocation = FVector(InitialPosition.X, InitialPosition.Y, InitialPosition.Z + 40.0f );
	DownLocation = FVector(InitialPosition.X, InitialPosition.Y, InitialPosition.Z - 40.0f);
	FromLocation = InitialPosition;
}

// Called every frame
void ABaseElementCube::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (bShowDebugInfo)
	{
		UE_LOG(LogTemp, Warning, TEXT("Current Location: X - %f, Y - %f, Z - %f"), FromLocation.X, FromLocation.Y, FromLocation.Z);
		UE_LOG(LogTemp, Warning, TEXT("Initial Position: X - %f, Y - %f, Z - %f"), InitialPosition.X, InitialPosition.Y, InitialPosition.Z);
		UE_LOG(LogTemp, Warning, TEXT("Up Location: X - %f, Y - %f, Z - %f"), UpLocation.X, UpLocation.Y, UpLocation.Z);
		UE_LOG(LogTemp, Warning, TEXT("My CubeX: %d; My CubeY: %d; My Index: %d;"), CubeX, CubeY, CubeIndex);
	}

	if (bSelected)
	{
		UE_LOG(LogTemp, Warning, TEXT("Default selected routine"));
		if (alpha <= 1.0f)
		{
			if (direction > 0)
				SetActorLocation(FMath::Lerp(FromLocation, UpLocation, alpha));
			else
				SetActorLocation(FMath::Lerp(FromLocation, DownLocation, alpha));

			alpha += (DeltaTime / 5);
		}
		else
		{
			direction *= -1;

			FromLocation = GetActorLocation();
			InitialPosition = FromLocation;
			UpLocation = FVector(FromLocation.X, FromLocation.Y, FromLocation.Z + 40.0f);
			DownLocation = FVector(FromLocation.X, FromLocation.Y, FromLocation.Z - 40.0f);

			alpha = 0;
		}
	}
}

void ABaseElementCube::OnClicked(UPrimitiveComponent* ClickedComp)
{
	AElementsTwoGameMode * MyGameMode = Cast<AElementsTwoGameMode>(UGameplayStatics::GetGameMode(this));
	if (MyGameMode->bInputEnabled)
	{
		ClickHandler(ClickedComp, MyGameMode);
	}
}

void ABaseElementCube::ClickHandler(UPrimitiveComponent* ClickedComp, AElementsTwoGameMode * OurGameMode)
{
	UE_LOG(LogTemp, Warning, TEXT("Clicked a cube, %s"), *GetName());

	if (OurGameMode->SelectedCube != NULL)
	{
		int DiffX = FMath::Abs(OurGameMode->SelectedCube->CubeX - CubeX);
		int DiffY = FMath::Abs(OurGameMode->SelectedCube->CubeY - CubeY);
		int Total = DiffX + DiffY;
		UE_LOG(LogTemp, Warning, TEXT("Differences X: %d; Y: %d;"), DiffX, DiffY);
		if (DiffX == 1 || DiffY == 1)
		{
			OurGameMode->SwapCubesInit(this);
		}
		else
		{
			OurGameMode->SelectedCube->bSelected = false;
			OurGameMode->SelectedCube->SelfToInitialPosition();
			bSelected = true;
			OurGameMode->SelectedCube = this;
		}
	}
	else
	{
		bSelected = true;
		OurGameMode->SelectedCube = this;
	}
	//bSelected = true;
	//OurGameMode->SelectedCube = this;
}

void ABaseElementCube::SelfToInitialPosition()
{
	SetActorLocation(InitialPosition);
}

void ABaseElementCube::StopHighlighting()
{
	bSelected = false;
	//SelfToInitialPosition();
}

void ABaseElementCube::OnSwapEnd()
{
	FromLocation = FVector( GetActorLocation().X, GetActorLocation().Y, 0.0f);
	InitialPosition = FromLocation;
	UpLocation = FVector(FromLocation.X, FromLocation.Y, FromLocation.Z + 40.0f);
	DownLocation = FVector(FromLocation.X, FromLocation.Y, FromLocation.Z - 40.0f);
	//FourInARow();
}

void ABaseElementCube::FourInARow()
{
	UE_LOG(LogTemp, Warning, TEXT("Let's determine four in a row"));
	TArray<int> bricksToKillIndexes;

	int index = CubeIndex;
}

void ABaseElementCube::MoveDown(int32 multiplier)
{
	FVector myPosition = GetActorLocation();
	FVector newPosition = FVector(myPosition.X - (differenceBetweenCenters * multiplier), myPosition.Y, myPosition.Z);

	SetActorLocation(newPosition);

	FromLocation = newPosition;
	InitialPosition = newPosition;
	UpLocation = FVector(newPosition.X, newPosition.Y, newPosition.Z + 40.0f);
	DownLocation = FVector(newPosition.X, newPosition.Y, newPosition.Z - 40.0f);
}

void ABaseElementCube::SpecialAction()
{
	int32 iScoreToTime = (scoreValue * iSpecialActionModifier) / 10;
	AElementsTwoGameMode * aCurrentGame = Cast<AElementsTwoGameMode>(UGameplayStatics::GetGameMode(this));
	aCurrentGame->AddToTimer(iScoreToTime);
}

void ABaseElementCube::SetSpecialActionModifier(int32 iModifier)
{
	iSpecialActionModifier = iModifier;
}
