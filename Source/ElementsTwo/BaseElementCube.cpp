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
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshComponent(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere")); //changed for debug purposes, change to cube after successfull debug

	if (MeshComponent.Succeeded())
	{
		VisibleCompponent->SetStaticMesh(MeshComponent.Object);
	}
	VisibleCompponent->AttachTo(RootComponent);
	VisibleCompponent->OnClicked.AddDynamic(this, &ABaseElementCube::OnClicked);
	differenceBetweenCenters = 110.0f;
	bAfterDestroyGenerated = false;
	scoreValue = 10;
	bDestroyed = false;
	iSpecialActionModifier = 1;
	bHighlighted = false;
	CollisionBoxHorizontal = CreateDefaultSubobject<UBoxComponent>(TEXT("HorizontalBox"));
	CollisionBoxHorizontal->AttachTo(VisibleCompponent);
	CollisionBoxHorizontal->SetBoxExtent(FVector(170.0f, 0.0f, 0.0f));
	CollisionBoxVertical = CreateAbstractDefaultSubobject<UBoxComponent>(TEXT("VerticalBox"));
	CollisionBoxVertical->AttachTo(VisibleCompponent);
	CollisionBoxVertical->SetBoxExtent(FVector(0.0f, 170.0f, 0.0f));
	bDownMovement = false;
}

void ABaseElementCube::HighlightMe(bool bHighlight)
{
	if (bHighlight != bHighlighted)
	{
		if (bHighlight)
		{
			static ConstructorHelpers::FObjectFinder<UMaterial> MyMaterial(TEXT("Material'/Game/StarterContent/Materials/M_Debug_Highlited_Girst.M_Debug_Highlited_Girst'"));
			if (MyMaterial.Succeeded())
			{
				VisibleCompponent->SetMaterial(1, MyMaterial.Object);
			}
		}
		else
		{
			if (DefaultMaterial)
			{
				VisibleCompponent->SetMaterial(0, DefaultMaterial);
			}
		}

		bHighlighted = bHighlight;
	}	
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

	AElementsTwoGameMode * gm = Cast<AElementsTwoGameMode>(UGameplayStatics::GetGameMode(this));

	if (CubeIndex - gm->bricksInARow > 0)
	{
		if (gm->ElemBricks[CubeIndex - gm->bricksInARow] == NULL)
		{
			int32 cordY = (CubeIndex - gm->bricksInARow) % gm->bricksInARow;
			int32 cordX = (CubeIndex - gm->bricksInARow) / gm->bricksInARow;
			targetPosition = FVector(cordX * 110.0f, cordY * 110.0f, 0.0f);
			bDownMovement = true;
		}
	}
	else
	{
		if (bDownMovement)
			bDownMovement = !bDownMovement;
	}

	if (bDownMovement)
	{
		MoveDownLerpSimple(DeltaTime);
	}
}

void ABaseElementCube::OnClicked(UPrimitiveComponent* ClickedComp)
{
	AElementsTwoGameMode * MyGameMode = Cast<AElementsTwoGameMode>(UGameplayStatics::GetGameMode(this));
	if (MyGameMode->bInputEnabled)
	{
		if(!bDownMovement)
			ClickHandler(ClickedComp, MyGameMode);
	}
}

void ABaseElementCube::ClickHandler(UPrimitiveComponent* ClickedComp, AElementsTwoGameMode * OurGameMode)
{
	if (OurGameMode->SelectedCube != NULL)
	{
		int DiffX = FMath::Abs(OurGameMode->SelectedCube->CubeIndex - CubeIndex);
		int DiffY = FMath::Abs(OurGameMode->SelectedCube->CubeIndex - CubeIndex);
		int Total = DiffX + DiffY;
		UE_LOG(LogTemp, Warning, TEXT("Differences X: %d; Y: %d;"), DiffX, DiffY);
		if (DiffX == 1 || DiffY == OurGameMode->bricksInARow)
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
		VisibleCompponent->SetRenderCustomDepth(true);
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

void ABaseElementCube::MoveDownLerp(int32 multiplier, bool bUseLerp)
{
	
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

void ABaseElementCube::MoveDownSet(FVector _targetPosition)
{
	bDownMovement = true;
	targetPosition = _targetPosition;
}

void ABaseElementCube::MoveDownLerpSimple(float delta)
{
	FVector currentLocation = GetActorLocation();
	if (FMath::Abs(FVector::Dist(currentLocation, targetPosition)) > 0.01f)
	{
		SetActorLocation(FMath::Lerp(currentLocation, targetPosition, delta * 10.0f));
	}
	else
	{
		AElementsTwoGameMode * gm = Cast<AElementsTwoGameMode>(UGameplayStatics::GetGameMode(this));
		//ABaseElementCube * swap = this;

		if (CubeIndex - gm->bricksInARow > 0)
		{
			int32 prevCubeY = CubeY;
			gm->ElemBricks[CubeIndex - gm->bricksInARow] = this;
			gm->ElemBricks[CubeIndex] = NULL;
			int32 prevIndex = CubeIndex;
			CubeIndex = CubeIndex - gm->bricksInARow;
			CubeX = CubeIndex % gm->bricksInARow;
			CubeY = CubeIndex / gm->bricksInARow;
			InitialPosition = GetActorLocation();
			if (prevCubeY == 9)
			{
				gm->EmptyIndexes.Add(prevIndex);
			}
		}
		bDownMovement = false;
	}
}

void ABaseElementCube::MoveDownABlock(AElementsTwoGameMode * GameMode)
{
	float targetY = CubeY - 1 * 110.0f;
	FVector currentLoc = GetActorLocation();
	FVector targetLoc = FVector(CubeX * 110.0f, targetY, 0.0f);
	if (FMath::Abs(FVector::Dist(currentLoc, targetLoc)) > 0.01f)
	{
		SetActorLocation(FMath::Lerp(currentLoc, targetLoc, 0.1f));
		InitialPosition = GetActorLocation();
		FromLocation = InitialPosition;
		UpLocation = FVector(InitialPosition.X, InitialPosition.Y, InitialPosition.Z + 40.0f);
		DownLocation = FVector(InitialPosition.X, InitialPosition.Y, InitialPosition.Z - 40.0f);
	}
	else
	{
		bDownMovement = false; // maybe unnecessary
	}
}

void ABaseElementCube::MoveDownSetI(int32 modifier)
{
	if (CubeY - modifier >= 0)
	{

	}
}

void ABaseElementCube::EndPlay(const EEndPlayReason::Type EndReason)
{
	AElementsTwoGameMode * gm = Cast<AElementsTwoGameMode>(UGameplayStatics::GetGameMode(this));

	gm->highScore += scoreValue;
}
