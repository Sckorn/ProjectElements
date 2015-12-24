// Fill out your copyright notice in the Description page of Project Settings.

#include "ElementsTwo.h"
#include "BaseElementCube.h"
#include "IceElementCube.h"
#include "ElectricElementCube.h"
#include "AcidElementCube.h"
#include "FireElementCube.h"
#include "ElementsPlayerController.h"
#include "ElementsTwoGameMode.h"
#include <stdexcept>


AElementsTwoGameMode::AElementsTwoGameMode()
{
	bricksInARow = 10;
	totalBricksToSpawn = 100;
	totalBrickTypes = 4;
	DefaultPawnClass = NULL;
	PlayerControllerClass = AElementsPlayerController::StaticClass();
	bInputEnabled = true;
	alpha = 0.0f;
	//CubesRows.Init(row, rowsNum);
	

};

void AElementsTwoGameMode::Tick(float DeltaTime)
{
	//UE_LOG(LogTemp, Warning, TEXT("Am I Ticking?"));
	//return;
	if (bSwapStarted)
	{
		if (alpha <= 1.0f)
		{
			if (SelectedCube != NULL)
			{
				SelectedCube->SetActorLocation(FMath::Lerp(SelectedCube->GetActorLocation(), SecondCubeInitialPosition, alpha));
			}

			if (SecondSelectedCube != NULL)
			{
				SecondSelectedCube->SetActorLocation(FMath::Lerp(SecondSelectedCube->GetActorLocation(), FirstCubeInitialPosition, alpha));
			}

			alpha += (DeltaTime / 5);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Resetting"));
			bSwapStarted = false;
			bInputEnabled = true;
			alpha = 0.0f;
			FVector TmpLoc = SelectedCube->GetActorLocation();
			SelectedCube->SetActorLocation(FVector(TmpLoc.X, TmpLoc.Y, 0.0f));
			SelectedCube->OnSwapEnd();
			

			TmpLoc = SecondSelectedCube->GetActorLocation();
			SecondSelectedCube->SetActorLocation(FVector(TmpLoc.X, TmpLoc.Y, 0.0f));
			SecondSelectedCube->OnSwapEnd();			
			
			SwapCoordinates();
			FindFourInARow(ElemBricks[SelectedCube->CubeIndex]->CubeIndex);
			SelectedCube = NULL;
			SecondSelectedCube = NULL;
		}
	}
}

void AElementsTwoGameMode::BeginPlay()
{
	Super::BeginPlay();
	int currentRow = 0;
	int k = 0;
	for (int i = 0; i < totalBricksToSpawn / bricksInARow; i++)
	{
		for (int j = 0; j < bricksInARow; j++)
		{
			ElemBricks.AddDefaulted();
			if (ElemBricks.IsValidIndex(k))
			{
				EElementType randd = (EElementType)FMath::RandRange(0, 3);

				randd = DetectFourInARow(k, randd);

				switch (randd)
				{
				case EElementType::EIce: ElemBricks[k] = GWorld->SpawnActor<AIceElementCube>(AIceElementCube::StaticClass()); break;
				case EElementType::EFire: ElemBricks[k] = GWorld->SpawnActor<AFireElementCube>(AFireElementCube::StaticClass()); break;
				case EElementType::EElectricity: ElemBricks[k] = GWorld->SpawnActor<AElectricElementCube>(AElectricElementCube::StaticClass()); break;
				case EElementType::EAcid: ElemBricks[k] = GWorld->SpawnActor<AAcidElementCube>(AAcidElementCube::StaticClass());  break;

				}

				/*if (randd == 0)
					ElemBricks[i] = GWorld->SpawnActor<AIceElementCube>(AIceElementCube::StaticClass());
				else
				if (randd == 1)
					ElemBricks[i] = GWorld->SpawnActor<AElectricElementCube>(AElectricElementCube::StaticClass());
				else
					ElemBricks[i] = GWorld->SpawnActor<AAcidElementCube>(AAcidElementCube::StaticClass());*/
				ElemBricks[k]->CubeX = j;
				ElemBricks[k]->CubeY = currentRow;
				ElemBricks[k]->CubeIndex = k;
				ElemBricks[k]->SetActorLocation(FVector(110.0f * currentRow, (110.0f * j), 0.0f));
				k++;
			}
		}
		++currentRow;
	}	
}

void AElementsTwoGameMode::SwapCubesInit(ABaseElementCube * SecondSelected)
{
	bSwapStarted = true;
	bInputEnabled = false;
	SecondSelectedCube = SecondSelected;
	FirstCubeInitialPosition = SelectedCube->GetActorLocation();
	SecondCubeInitialPosition = SecondSelected->GetActorLocation();
	SelectedCube->StopHighlighting();
	SecondSelectedCube->StopHighlighting();
	/*SelectedCube->SetActorLocation(SecondCubeInitialPosition);
	SecondSelectedCube->SetActorLocation(FirstCubeInitialPosition);*/
}

EElementType AElementsTwoGameMode::DetectFourInARow(int32 index, EElementType type)
{
	/*checking horizontal bricks before current, three of them have to valid*/

	int counter = 0;
	EElementType typeBeforeFirstLoop = type;
	for (int i = index - 1; i > index - 4 && i > 0; i--)
	{
		if (ElemBricks[i]->CubeX > index) break;
		if (ElemBricks[i]->BrickType != type) break;
		counter++;
	}

	if (counter == 3)
	{
		/*uint8 typeTemp = (uint8)type;
		typeTemp++;
		if (typeTemp > totalBrickTypes - 1)
		{
			typeTemp = 0;
		}
		type = (EElementType)typeTemp;*/
		type = GetNextAvailableType(type);
		/*if (type == typeBeforeFirstLoop)
		{
			type = GetNextAvailableType(type);
		}*/
	}

	counter = 0;
	EElementType typeBeforeSecondLoop = type;
	for (int i = index - bricksInARow; i > index - (bricksInARow * 4) && i > 0; i -= bricksInARow)
	{
		if (ElemBricks[i]->BrickType != type) break;
		counter++;
	}

	if (counter == 3)
	{
		/*uint8 typeTemp = (uint8)type;
		typeTemp++;
		if (typeTemp > totalBrickTypes)
		{
			typeTemp = 0;
		}
		type = (EElementType)typeTemp;*/
		type = GetNextAvailableType(type);
		if (type == typeBeforeFirstLoop)
		{
			type = GetNextAvailableType(type);
			if (type == typeBeforeSecondLoop)
			{
				type = GetNextAvailableType(type);
			}
		}
	}

	return type;
}

EElementType AElementsTwoGameMode::GetNextAvailableType(EElementType currentType)
{
	uint8 typeTemp = (uint8)currentType;
	typeTemp++;
	if (typeTemp > totalBrickTypes - 1)
	{
		typeTemp = 0;
	}
	currentType = (EElementType)typeTemp;
	return currentType;
}

void AElementsTwoGameMode::SwapCoordinates()
{
	int32 tempX = 0;
	int32 tempY = 0;
	int32 tempIndex = 0;
	UE_LOG(LogTemp, Warning, TEXT("Swap ended now switchong our coordinates"));
	ABaseElementCube * tmpCube = NULL;

	if (SelectedCube == NULL || SecondSelectedCube == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("Object NULL reference!"));
		return;
	}

	int32 firstCubeInitialIndex = SelectedCube->CubeIndex;
	int32 secondCubeInitialIndex = SecondSelectedCube->CubeIndex;
	UE_LOG(LogTemp, Warning, TEXT("Selected cube index %d, second selected cube index %d"), SelectedCube->CubeIndex, SecondSelectedCube->CubeIndex);
	//tempX = SelectedCube->CubeX;
	//tempY = SelectedCube->CubeY;
	//tempIndex = SelectedCube->CubeIndex;
	//tmpCube = ElemBricks[SelectedCube->CubeIndex];
	//ElemBricks[SelectedCube->CubeIndex] = SecondSelectedCube;
	//ElemBricks[SecondSelectedCube->CubeIndex] = tmpCube;

	/*SelectedCube->CubeX = SecondSelectedCube->CubeX;
	SelectedCube->CubeY = SecondSelectedCube->CubeY;
	SelectedCube->CubeIndex = SecondSelectedCube->CubeIndex;
	
	SecondSelectedCube->CubeX = tempX;
	SecondSelectedCube->CubeY = tempY;
	SecondSelectedCube->CubeIndex = tempIndex;*/
		
	tempX = ElemBricks[SelectedCube->CubeIndex]->CubeX;
	tempY = ElemBricks[SelectedCube->CubeIndex]->CubeY;
	tempIndex = ElemBricks[SelectedCube->CubeIndex]->CubeIndex;

	ElemBricks[SelectedCube->CubeIndex]->CubeX = ElemBricks[SecondSelectedCube->CubeIndex]->CubeX;
	ElemBricks[SelectedCube->CubeIndex]->CubeY = ElemBricks[SecondSelectedCube->CubeIndex]->CubeY;
	ElemBricks[SelectedCube->CubeIndex]->CubeIndex = ElemBricks[SecondSelectedCube->CubeIndex]->CubeIndex;

	ElemBricks[SecondSelectedCube->CubeIndex]->CubeX = tempX;
	ElemBricks[SecondSelectedCube->CubeIndex]->CubeY = tempY;
	ElemBricks[SecondSelectedCube->CubeIndex]->CubeIndex = tempIndex;

	tmpCube = ElemBricks[firstCubeInitialIndex];
	ElemBricks[firstCubeInitialIndex] = ElemBricks[secondCubeInitialIndex];
	ElemBricks[secondCubeInitialIndex] = tmpCube;
}

void AElementsTwoGameMode::FindFourInARow(int32 index)
{
	TArray<int32> bricksToDestroyIndexesHorizontal;
	bricksToDestroyIndexesHorizontal.Add(index);

	if (index < 0 || index >= totalBricksToSpawn) { return; }

	for (int32 i = index - 1; i > index - 4 && i > 0; i--)
	{
		if (ElemBricks[i]->CubeX > ElemBricks[index]->CubeX) break;
		if (ElemBricks[i]->BrickType == ElemBricks[index]->BrickType) bricksToDestroyIndexesHorizontal.Add(i);
		else
		{
			break;
		}
	}

	for (int32 i = index + 1; i < index + 4 && i < totalBricksToSpawn; i++)
	{
		if (ElemBricks[i]->CubeX < ElemBricks[index]->CubeX) break;
		if (ElemBricks[i]->BrickType == ElemBricks[index]->BrickType) bricksToDestroyIndexesHorizontal.Add(i);
		else
		{
			break;
		}
	}

	TArray<int32> bricksToDestroyIndexesVertical;

	for (int32 i = index - bricksInARow; i > index - (bricksInARow * 4) && i > 0; i -= bricksInARow)
	{
		if (ElemBricks[i]->BrickType == ElemBricks[index]->BrickType) bricksToDestroyIndexesVertical.Add(i);
		else
		{
			break;
		}
	}

	for (int32 i = index + bricksInARow; i < index + (bricksInARow * 4) && i < totalBricksToSpawn; i += bricksInARow)
	{
		if (ElemBricks[i]->BrickType == ElemBricks[index]->BrickType) bricksToDestroyIndexesVertical.Add(i);
		else
		{
			break;
		}
	}

	if (bricksToDestroyIndexesHorizontal.Num() >= 4)
	{
		DeleteCubes(bricksToDestroyIndexesHorizontal);
	}
	else
	{
		bricksToDestroyIndexesVertical.Add(index);
	}

	if (bricksToDestroyIndexesVertical.Num() + 1 >= 4)
	{
		DeleteCubes(bricksToDestroyIndexesVertical);
	}
}

void AElementsTwoGameMode::DeleteCubes(const TArray<int32> toDelete)
{
	for (int32 i = 0; i < toDelete.Num(); i++)
	{
		ElemBricks[toDelete[i]]->Destroy();
	}
}

