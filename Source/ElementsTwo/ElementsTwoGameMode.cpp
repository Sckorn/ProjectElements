// Fill out your copyright notice in the Description page of Project Settings.

#include "ElementsTwo.h"
#include "BaseElementCube.h"
#include "IceElementCube.h"
#include "ElectricElementCube.h"
#include "AcidElementCube.h"
#include "FireElementCube.h"
#include "ElementsPlayerController.h"
#include "ElementsTwoGameMode.h"
#include "ElementsHUD.h"
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
	highScore = 0;
	HUDClass = AElementsHUD::StaticClass();
	bGameActive = true;
	iCountdownTime = 90;
	iPauseCountdownTime = 5;
	bDeleteSequenceStarted = false;
	tmpIndex = -1;
	moveDownAlpha = 0.0f;
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
			//FindFourInARow(ElemBricks[SecondSelectedCube->CubeIndex]->CubeIndex);
			SelectedCube = NULL;
			SecondSelectedCube = NULL;
		}
	}

	//cube delete sequence
	if (bDeleteSequenceStarted)
	{
		PauseMainTimer();

		/*

		horizontal destruction

		*/
		if (CubeIndexesToDeleteHorizontal.Num() > 0)
		{
			if (currentDeletingIndex < CubeIndexesToDeleteHorizontal.Num())
			{
				UE_LOG(LogTemp, Warning, TEXT("Current deleting index %d, actual value %d"), currentDeletingIndex, CubeIndexesToDeleteHorizontal[currentDeletingIndex]);
				if (tmpIndex == -1)
				{
					tmpIndex = CubeIndexesToDeleteHorizontal[currentDeletingIndex];
					pDelete = ElemBricks[tmpIndex];
					UE_LOG(LogTemp, Warning, TEXT("Tmp index %d"), tmpIndex);
				}

				if (!ElemBricks[CubeIndexesToDeleteHorizontal[currentDeletingIndex]]->bDestroyed)
				{
					UE_LOG(LogTemp, Warning, TEXT("Current tmpIndex %d"), tmpIndex);
					if (!pDelete->bDestroyed)
					{
						//ElemBricks[pDelete->CubeIndex]->SetActorHiddenInGame(true);
						pDelete->SetActorHiddenInGame(true);
					}

					UE_LOG(LogTemp, Warning, TEXT("tmpIndex + bricksInARow = %d"), (tmpIndex + bricksInARow));
					if (tmpIndex + bricksInARow < totalBricksToSpawn)
					{

						FVector upperCubeLocation = ElemBricks[tmpIndex + bricksInARow]->GetActorLocation();
						FVector lowerCubeLocation = ElemBricks[tmpIndex]->InitialPosition;//->GetActorLocation();
						UE_LOG(LogTemp, Warning, TEXT("Remaining distance %f"), FVector::Dist(lowerCubeLocation, upperCubeLocation));
						if (FVector::Dist(lowerCubeLocation, upperCubeLocation) > 5.0f)
						{
							ElemBricks[tmpIndex + bricksInARow]->SetActorLocation(FMath::Lerp(upperCubeLocation, lowerCubeLocation, DeltaTime * 10));
							moveDownAlpha += DeltaTime * 10;
						}
						else
						{
							ElemBricks[tmpIndex] = ElemBricks[tmpIndex + bricksInARow];
							ElemBricks[tmpIndex]->CubeIndex = tmpIndex;
							ElemBricks[tmpIndex]->CubeX = tmpIndex % 10;
							ElemBricks[tmpIndex]->CubeY = tmpIndex / 10;
							ElemBricks[tmpIndex]->OnSwapEnd();

							UE_LOG(LogTemp, Warning, TEXT("Lerping to lower position finished"));
							if (tmpIndex + bricksInARow < totalBricksToSpawn)
							{
								tmpIndex += bricksInARow;
							}
							else
							{
								//ElemBricks[CubeIndexesToDeleteHorizontal[currentDeletingIndex]]->bDestroyed = true;
								//ElemBricks[CubeIndexesToDeleteHorizontal[currentDeletingIndex]]->Destroy();
								/*UE_LOG(LogTemp, Error, TEXT("Destroying object %s"), *pDelete->GetName());*/
								pDelete->bDestroyed = true;
								pDelete->Destroy();

								currentDeletingIndex += 1;
								tmpIndex = -1;
							}

							moveDownAlpha = 0.0f;
						}
					}
					else
					{
						if (!pDelete->bDestroyed)
						{
							UE_LOG(LogTemp, Error, TEXT("Destroying object %s"), *pDelete->GetName());
							pDelete->bDestroyed = true;
							pDelete->Destroy();
						}
						currentDeletingIndex += 1;
						tmpIndex = -1;
					}
				}
				else
				{
					if (!pDelete->bDestroyed)
					{
						UE_LOG(LogTemp, Error, TEXT("Destroying object %s"), *pDelete->GetName());
						pDelete->bDestroyed = true;
						pDelete->Destroy();
					}
					currentDeletingIndex += 1;
					tmpIndex = -1;
				}
			}
			else
			{
				AfterDeleteHorizontalSpawner(CubeIndexesToDeleteHorizontal);
				CubeIndexesToDeleteHorizontal.Empty();
				//UE_LOG(LogTemp, Error, TEXT("How much bricks are left ? Around %d"), CubeIndexesToDeleteHorizontal.Num());
				currentDeletingIndex = 0;
				tmpIndex = -1;
			}
		}

		/*

		horizontal destruction end

		*/

		/*

		vertical destruction

		*/

		if (CubeIndexesToDeleteHorizontal.Num() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Horizontal indexes amount %d"), CubeIndexesToDeleteHorizontal.Num());
			UE_LOG(LogTemp, Warning, TEXT("current deleting index %d,total indexes %d"), currentDeletingIndex, CubeIndexesToDeleteVertical.Num());
			//if (currentDeletingIndex < CubeIndexesToDeleteVertical.Num())
			if(CubeIndexesToDeleteVertical.Num() > 0)
			{
				if (tmpIndex == -1)
				{
					tmpIndex = CubeIndexesToDeleteVertical[CubeIndexesToDeleteVertical.Num() - 1] + bricksInARow;
					startedIndex = tmpIndex;
					lowestIndex = CubeIndexesToDeleteVertical[0];
				}

				if (tmpIndex < totalBricksToSpawn)
				{
					FVector myLoc = ElemBricks[tmpIndex]->GetActorLocation();
					FVector targLoc = FVector((ElemBricks[tmpIndex]->CubeY - 1) * 110.0f, ElemBricks[tmpIndex]->CubeX * 110.0f, 0.0f);
					if (tmpIndex > lowestIndex)
					{
						//float dist = FVector::Dist(myLoc, targLoc);
						UE_LOG(LogTemp, Warning, TEXT( "TmpIndex: %d; lowestIndex: %d; Distance: %f"), tmpIndex, lowestIndex, FVector::Dist(myLoc, targLoc));
						if (FMath::Abs(FVector::Dist(myLoc, targLoc)) < 0.1f)
						{
							ElemBricks[tmpIndex - bricksInARow] = ElemBricks[tmpIndex];
							ElemBricks[tmpIndex - bricksInARow]->CubeIndex = tmpIndex - bricksInARow;
							ElemBricks[tmpIndex - bricksInARow]->CubeX = ElemBricks[tmpIndex - bricksInARow]->CubeIndex % bricksInARow;
							ElemBricks[tmpIndex - bricksInARow]->CubeY = ElemBricks[tmpIndex - bricksInARow]->CubeIndex / bricksInARow;
							tmpIndex = tmpIndex - bricksInARow;
						}
						else
						{
							ElemBricks[tmpIndex]->SetActorLocation(FMath::Lerp(myLoc, targLoc, DeltaTime));
						}
					}
					else
					{
						lowestIndex += bricksInARow;
						startedIndex += bricksInARow;

						if (startedIndex > totalBricksToSpawn)
						{
							CubeIndexesToDeleteVertical.Empty();
						}

						tmpIndex = startedIndex;
					}
				}
				else
				{
					tmpIndex = -1;
					lowestIndex = -1;
				}
			}
			else
			{
				tmpIndex = -1;
				currentDeletingIndex = -1;
			}
		}

		/*
		
		vertical destruction end
		
		*/
	}
}

void AElementsTwoGameMode::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(fTimerHandle, this, &AElementsTwoGameMode::AdvanceTimer, 1.0f, true);
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
		UE_LOG(LogTemp, Warning, TEXT("First loop"));
		UE_LOG(LogTemp, Warning, TEXT("ElemBrick[%d] CubeX: %d; ElemBrickp[%d] CubeX: %d;"), i, ElemBricks[i]->CubeX, index, ElemBricks[index]->CubeX);
		if (ElemBricks[i]->CubeX > ElemBricks[index]->CubeX) break;
		if (ElemBricks[i]->BrickType == ElemBricks[index]->BrickType) bricksToDestroyIndexesHorizontal.Add(i);
		else
		{
			break;
		}
	}

	for (int32 i = index + 1; i < index + 4 && i < totalBricksToSpawn; i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("Second loop"));
		UE_LOG(LogTemp, Warning, TEXT("ElemBrick[%d] CubeX: %d; ElemBrickp[%d] CubeX: %d;"), i, ElemBricks[i]->CubeX, index, ElemBricks[index]->CubeX);
		if (ElemBricks[i]->CubeX < ElemBricks[index]->CubeX) break;
		if (ElemBricks[i]->BrickType == ElemBricks[index]->BrickType) bricksToDestroyIndexesHorizontal.Add(i);
		else
		{
			break;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Horizontal sequence of %d elements found"), bricksToDestroyIndexesHorizontal.Num());

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

	UE_LOG(LogTemp, Warning, TEXT("Vertical sequence of %d elements found"), bricksToDestroyIndexesVertical.Num());

	if (bricksToDestroyIndexesHorizontal.Num() >= 4)
	{
		//DeleteCubes(bricksToDestroyIndexesHorizontal);
		bDeleteSequenceStarted = true;
		bricksToDestroyIndexesHorizontal.Sort([](int a, int b) { return a < b; });

		CubeIndexesToDeleteHorizontal = bricksToDestroyIndexesHorizontal;
	}
	else
	{
		bricksToDestroyIndexesVertical.Add(index);
	}

	if (bricksToDestroyIndexesVertical.Num() + 1 >= 4)
	{
		bDeleteSequenceStarted = true;
		bricksToDestroyIndexesVertical.Sort([](int a, int b) {return a < b; });
		for (int32 i = 0; i < bricksToDestroyIndexesVertical.Num(); i++)
		{
			UE_LOG(LogTemp, Warning, TEXT("bVertc[%d] = %d"), i, bricksToDestroyIndexesVertical[i]);
			ElemBricks[bricksToDestroyIndexesVertical[i]]->SetActorHiddenInGame(true);
		}
		CubeIndexesToDeleteVertical = bricksToDestroyIndexesVertical;
		verticalSequenceAmount = CubeIndexesToDeleteVertical.Num();
		//DeleteCubes(bricksToDestroyIndexesVertical);
	}
}

void AElementsTwoGameMode::DeleteCubes(const TArray<int32> toDelete)
{
	ElemBricks[toDelete[0]]->SetSpecialActionModifier(toDelete.Num());
	ElemBricks[toDelete[0]]->SpecialAction();
	for (int32 i = 0; i < toDelete.Num(); i++)
	{
		highScore += ElemBricks[i]->scoreValue;
		ElemBricks[toDelete[i]]->bDestroyed = true;
		ElemBricks[toDelete[i]]->Destroy();
	}

	AfterCubesDelete(toDelete);
}

void AElementsTwoGameMode::DeleteCubesLerpMode(const TArray<int32> toDelete)
{
	for (int32 i = 0; i < toDelete.Num(); i++)
	{
		ElemBricks[toDelete[i]]->VisibleCompponent->ToggleVisibility();
	}
}

void AElementsTwoGameMode::StartDeletingSequence(const TArray<int32> toDelete)
{
	/*bDeleteSequenceStarted = true;
	CubeIndexesToDelete = toDelete;
	currentDeletingIndex = 0;*/
}

void AElementsTwoGameMode::AfterCubesDelete(const TArray<int32> deletedIndexes)
{
	bool bVerticalFlag = false;

	if (FMath::Abs(deletedIndexes[0] - deletedIndexes[1]) > 1) bVerticalFlag = true;

	if (bVerticalFlag)
	{
		UE_LOG(LogTemp, Warning, TEXT("Vertical sequence deleted"));

		TArray<int32> swapArr;
		swapArr = deletedIndexes;
		swapArr.Sort([](const int32& val1, const int32& val2) {return (val1 < val2); });

		int32 sequence = swapArr.Num();

		for (int32 i = 0; i < sequence; i++)
		{
			UE_LOG(LogTemp, Warning, TEXT("Index to delete %d"), deletedIndexes[i]);
		}

		for (int32 i = 0; i < sequence; i++)
		{
			UE_LOG(LogTemp, Warning, TEXT("Index to delete %d"), swapArr[i]);
		}

		int32 minIndex = swapArr[0];
		int32 maxIndex = swapArr[swapArr.Num() - 1];

		int32 secondDigit = maxIndex % 10;

		int32 highestBrickIndex = 90 + secondDigit;

		UE_LOG(LogTemp, Warning, TEXT("Minimum Index %d, Maximum index %d, second digit %d, highest brick index %d"), minIndex, maxIndex, secondDigit, highestBrickIndex);

		if (highestBrickIndex >= totalBricksToSpawn) highestBrickIndex -= (highestBrickIndex - totalBricksToSpawn);

		UE_LOG(LogTemp, Warning, TEXT("Minimum Index %d, Maximum index %d, second digit %d, highest brick index %d"), minIndex, maxIndex, secondDigit, highestBrickIndex);

		int32 modifier = 1;
		int32 j = maxIndex + (bricksInARow * modifier);
		int32 lastLowestCubeIndex = 0;

		//UE_LOG(LogTemp, Warning, TEXT("Max index %d, Min index %d, resulting j %d"), maxIndex, minIndex, j);
		while (j < totalBricksToSpawn)
		{
			UE_LOG(LogTemp, Warning, TEXT("Max index %d, Min index %d, resulting j %d"), maxIndex, minIndex, j);
			while (j > minIndex)
			{
				int32 lowerCube = j - bricksInARow;
				ElemBricks[lowerCube] = ElemBricks[j];
				ElemBricks[lowerCube]->MoveDown();
				ElemBricks[lowerCube]->CubeIndex = lowerCube;
				ElemBricks[lowerCube]->CubeY = ElemBricks[lowerCube]->CubeIndex / bricksInARow;
				j = lowerCube;
			}

			minIndex += bricksInARow;
			modifier += 1;
			lastLowestCubeIndex = j;
			j = maxIndex + (bricksInARow * modifier);
		}

		UE_LOG(LogTemp, Warning, TEXT("Last lowest cbe index %d"), lastLowestCubeIndex);

		for (int32 i = lastLowestCubeIndex + bricksInARow; i < totalBricksToSpawn; i += bricksInARow)
		{
			EElementType randd = (EElementType)FMath::RandRange(0, 3);

			randd = DetectFourInARow(i, randd);

			switch (randd)
			{
			case EElementType::EIce: ElemBricks[i] = GWorld->SpawnActor<AIceElementCube>(AIceElementCube::StaticClass()); break;
			case EElementType::EFire: ElemBricks[i] = GWorld->SpawnActor<AFireElementCube>(AFireElementCube::StaticClass()); break;
			case EElementType::EElectricity: ElemBricks[i] = GWorld->SpawnActor<AElectricElementCube>(AElectricElementCube::StaticClass()); break;
			case EElementType::EAcid: ElemBricks[i] = GWorld->SpawnActor<AAcidElementCube>(AAcidElementCube::StaticClass());  break;

			}

			ElemBricks[i]->CubeX = i % bricksInARow;
			ElemBricks[i]->CubeY = i / bricksInARow;
			ElemBricks[i]->CubeIndex = i;
			ElemBricks[i]->SetActorLocation(FVector(110.0f * (i / bricksInARow), 110.0f * (j % bricksInARow), 0.0f));
			ElemBricks[i]->bAfterDestroyGenerated = true;
		}

		/*for (int32 i = maxIndex + bricksInARow; i < sequence; i++)
		{

		}*/
		/*for (int32 i = 0; i < sequence; i++)
		{
			UE_LOG(LogTemp, Warning, TEXT("For loop iteration start, deleted index %d"), swapArr[i]);
			int32 j = swapArr[i];

			if (j < 0 || j > totalBricksToSpawn) continue;

			int32 initialJ = j;
			while (j < totalBricksToSpawn) // while brick with current index is inside the borders
			{
				int32 upperBrickIndex = j + bricksInARow;
				if (upperBrickIndex >= totalBricksToSpawn) break; // if next cube is out of borders stop this cycle
				
				if (ElemBricks[upperBrickIndex] == nullptr)
				{
					UE_LOG(LogTemp, Warning, TEXT("Brick with index %d is destroyed"), upperBrickIndex);
					j = upperBrickIndex;
					continue;
				}

				UE_LOG(LogTemp, Warning, TEXT("Initial J %d, current j %d, upperBrick index %d, boolean operation result %d"), initialJ, j, upperBrickIndex, (ElemBricks[upperBrickIndex]->bDestroyed) ? 1 : 0);

				ElemBricks[initialJ] = ElemBricks[upperBrickIndex];				
				ElemBricks[initialJ]->CubeIndex = initialJ;
				ElemBricks[initialJ]->CubeY = initialJ / 10;
				int32 rowNumber = initialJ / 10;
				int32 colNumber = initialJ % 10;
				ElemBricks[initialJ]->SetActorLocation(FVector(110.0f * rowNumber, 110.0f * colNumber, 0.0f));

				UE_LOG(LogTemp, Warning, TEXT("Moved brick from %d to %d"), upperBrickIndex, initialJ);

				initialJ = upperBrickIndex;
				j = upperBrickIndex;
			}
			while (j < totalBricksToSpawn)
			{
				if (j + bricksInARow < totalBricksToSpawn)
				{
					if (ElemBricks[j + bricksInARow] != NULL)
					{
						ElemBricks[initialJ] = ElemBricks[j + bricksInARow];
						ElemBricks[initialJ]->CubeIndex = initialJ;
						ElemBricks[initialJ]->CubeY = ElemBricks[j]->CubeY - (1 * (j / bricksInARow));
						ElemBricks[initialJ]->MoveDown(FMath::Abs(initialJ - j));
						
						j = j + bricksInARow;
						initialJ = j;
					}
					else
					{
						j = j + bricksInARow;
					}
				}
				else
				{
					break;
				}
			}

			UE_LOG(LogTemp, Warning, TEXT("For loop iteration end, deleted index %d"), swapArr[i]);
		}*/
	}
	else
	{
		int32 sequence = deletedIndexes.Num();
		
		for (int32 i = 0; i < sequence; i++)
		{
			int32 j = deletedIndexes[i];

			if (j < 0 || j > totalBricksToSpawn) continue;

			while (j < totalBricksToSpawn)
			{
				if (j + bricksInARow < totalBricksToSpawn)
				{
					ElemBricks[j] = ElemBricks[j + bricksInARow];
					ElemBricks[j]->CubeIndex = j;
					ElemBricks[j]->CubeY = ElemBricks[j]->CubeY - 1;
					ElemBricks[j]->MoveDown();
					j = j + bricksInARow;
				}
				else
				{
					break;
				}
			}

			j = j; //- bricksInARow;

			EElementType randd = (EElementType)FMath::RandRange(0, 3);

			randd = DetectFourInARow(j, randd);

			switch (randd)
			{
			case EElementType::EIce: ElemBricks[j] = GWorld->SpawnActor<AIceElementCube>(AIceElementCube::StaticClass()); break;
			case EElementType::EFire: ElemBricks[j] = GWorld->SpawnActor<AFireElementCube>(AFireElementCube::StaticClass()); break;
			case EElementType::EElectricity: ElemBricks[j] = GWorld->SpawnActor<AElectricElementCube>(AElectricElementCube::StaticClass()); break;
			case EElementType::EAcid: ElemBricks[j] = GWorld->SpawnActor<AAcidElementCube>(AAcidElementCube::StaticClass());  break;

			}
			
			ElemBricks[j]->CubeX = j % bricksInARow;
			ElemBricks[j]->CubeY = 9;
			ElemBricks[j]->CubeIndex = j;
			ElemBricks[j]->SetActorLocation(FVector(110.0f * 9, 110.0f * (j % bricksInARow), 0.0f));
			ElemBricks[j]->bAfterDestroyGenerated = true;
		}		
	}
}

void AElementsTwoGameMode::AdvanceTimer()
{
	--iCountdownTime;
	if (iCountdownTime < 1)
	{
		GetWorldTimerManager().ClearTimer(fTimerHandle);
		OnCountdownEnd();
	}
}

void AElementsTwoGameMode::OnCountdownEnd()
{
	bGameActive = false;
}

void AElementsTwoGameMode::AdvancePauseTimer()
{
	--iPauseCountdownTime;
	if (iPauseCountdownTime < 1)
	{
		GetWorldTimerManager().ClearTimer(fPauseTimerHandle);
		OnPauseCountdownEnd();
	}
}

void AElementsTwoGameMode::OnPauseCountdownEnd()
{	
	if (GetWorldTimerManager().IsTimerPaused(fTimerHandle))
	{
		GetWorldTimerManager().UnPauseTimer(fTimerHandle);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Timer wasn't paused when OnPauseCountdownEnd was invoked!"));
	}
}

void AElementsTwoGameMode::AddToTimer(int32 iTimeToAdd)
{
	iCountdownTime += iTimeToAdd;
}

void AElementsTwoGameMode::PauseMainTimer()
{
	if (GetWorldTimerManager().IsTimerActive(fTimerHandle))
	{
		GetWorldTimerManager().PauseTimer(fTimerHandle);
		UE_LOG(LogTemp, Warning, TEXT("Paused the bitch."));
		//GetWorldTimerManager().SetTimer(fPauseTimerHandle, this, &AElementsTwoGameMode::AdvancePauseTimer, 1.0f, true);
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("The bitch is already inactive!"));
	}
}

void AElementsTwoGameMode::ResumeMainTimer()
{
	if (GetWorldTimerManager().IsTimerPaused(fTimerHandle))
	{
		GetWorldTimerManager().PauseTimer(fTimerHandle);
		UE_LOG(LogTemp, Warning, TEXT("Resumed the bitch."));
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("The bitch is not even paused!"));
	}
}

void AElementsTwoGameMode::AfterDeleteHorizontalSpawner(TArray<int32> deletedIndexes)
{
	for (int32 i = 0; i < deletedIndexes.Num(); i++)
	{
		int32 xPart = deletedIndexes[i] % 10;
		int32 toSpawnAt = xPart + 90;
		SpawnCubeAtPosition(toSpawnAt);
	}
}

void AElementsTwoGameMode::SpawnCubeAtPosition(int32 indexToSpawnAt)
{
	EElementType randd = (EElementType)FMath::RandRange(0, 3);

	randd = DetectFourInARow(indexToSpawnAt, randd);

	switch (randd)
	{
	case EElementType::EIce: ElemBricks[indexToSpawnAt] = GWorld->SpawnActor<AIceElementCube>(AIceElementCube::StaticClass()); break;
	case EElementType::EFire: ElemBricks[indexToSpawnAt] = GWorld->SpawnActor<AFireElementCube>(AFireElementCube::StaticClass()); break;
	case EElementType::EElectricity: ElemBricks[indexToSpawnAt] = GWorld->SpawnActor<AElectricElementCube>(AElectricElementCube::StaticClass()); break;
	case EElementType::EAcid: ElemBricks[indexToSpawnAt] = GWorld->SpawnActor<AAcidElementCube>(AAcidElementCube::StaticClass());  break;

	}

	ElemBricks[indexToSpawnAt]->CubeX = indexToSpawnAt % bricksInARow;
	ElemBricks[indexToSpawnAt]->CubeY = indexToSpawnAt / bricksInARow;
	ElemBricks[indexToSpawnAt]->CubeIndex = indexToSpawnAt;
	ElemBricks[indexToSpawnAt]->SetActorLocation(FVector(110.0f * (indexToSpawnAt / bricksInARow), 110.0f * (indexToSpawnAt % bricksInARow), 0.0f));
	ElemBricks[indexToSpawnAt]->bAfterDestroyGenerated = true;
}

/*void AElementsTwoGameMode::PerformSpecialAction()
{
	
}*/