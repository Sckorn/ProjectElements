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
			//FindFourInARow(ElemBricks[SelectedCube->CubeIndex]->CubeIndex);
			//FindFourInARow(ElemBricks[SecondSelectedCube->CubeIndex]->CubeIndex);
			SequenceDetector(ElemBricks[SelectedCube->CubeIndex]);
			SelectedCube = NULL;
			SecondSelectedCube = NULL;
		}
	}

	if (EmptyIndexes.Num() > 0)
	{
		for (int i = 0; i < EmptyIndexes.Num(); i++)
		{
			SpawnCubeAtPosition(EmptyIndexes[i]);
		}

		EmptyIndexes.Empty();
		ResumeMainTimer();
	}

	//cube delete sequence
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
		if (i > 0 && i < totalBricksToSpawn)
		{
			if (ElemBricks[i] != NULL)
			{
				if (ElemBricks[i]->CubeX > index) break;
				if (ElemBricks[i]->BrickType != type) break;
				counter++;
			}
		}
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
		if (i > 0 && i < totalBricksToSpawn)
		{
			if (ElemBricks[i] != NULL)
			{
				if (ElemBricks[i]->BrickType != type) break;
				counter++;
			}
		}
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

			//j = j; //- bricksInARow;

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
		GetWorldTimerManager().UnPauseTimer(fTimerHandle);
		UE_LOG(LogTemp, Warning, TEXT("Resumed the bitch."));
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("The bitch is not even paused!"));
	}

	//GetWorldTimerManager().UnPauseTimer(fTimerHandle);
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

void AElementsTwoGameMode::SequenceDetector(ABaseElementCube * selCb)
{
	PauseMainTimer();
	UE_LOG(LogTemp, Warning, TEXT("Clicked a cube, %s"), *selCb->GetName());

	TArray<AActor *> OvpActors;
	selCb->GetOverlappingActors(OvpActors);

	TArray<ABaseElementCube *> horizontalCubes;
	TArray<ABaseElementCube *> verticalCubes;

	for (int i = 0; i < OvpActors.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("Name: %s, type"), *OvpActors[i]->GetName());
		ABaseElementCube * bCube = Cast<ABaseElementCube>(OvpActors[i]);

		if (bCube->CubeX == selCb->CubeX)
			horizontalCubes.Add(bCube);

		if (bCube->CubeY == selCb->CubeY)
			verticalCubes.Add(bCube);
	}

	verticalCubes.Add(selCb);
	horizontalCubes.Add(selCb);

	UE_LOG(LogTemp, Warning, TEXT("Vertical sequence of %d cubes"), verticalCubes.Num());
	UE_LOG(LogTemp, Warning, TEXT("Horizontal sequence of %d cubes"), horizontalCubes.Num());

	verticalCubes.Sort([](const ABaseElementCube & a, const ABaseElementCube & b) {return a.CubeIndex < b.CubeIndex; });
	horizontalCubes.Sort([](const ABaseElementCube & a, const ABaseElementCube & b) {return a.CubeIndex < b.CubeIndex; });

	TArray<ABaseElementCube * > verticalSeq;
	for (int i = 0; i < verticalCubes.Num(); i++)
	{
		if (verticalCubes[i]->BrickType == selCb->BrickType)
		{
			verticalSeq.Add(verticalCubes[i]);
		}
		else
		{
			if (verticalSeq.Num() < 4)
				verticalSeq.Empty();
		}
	}

	TArray<ABaseElementCube *> horizontalSeq;
	for (int i = 0; i < horizontalCubes.Num(); i++)
	{
		if (horizontalCubes[i]->BrickType == selCb->BrickType)
		{
			horizontalSeq.Add(horizontalCubes[i]);
		}
		else
		{
			if (horizontalSeq.Num() < 4)
				horizontalSeq.Empty();
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Vertical sequence of %d elements"), verticalSeq.Num());
	UE_LOG(LogTemp, Warning, TEXT("Horizontal sequence of %d elements"), horizontalSeq.Num());

	if (verticalSeq.Num() >= 4)
	{
		for (int i = 0; i < verticalSeq.Num(); i++)
		{
			int32 elemBrickIndex = verticalSeq[i]->CubeIndex;
			ElemBricks[elemBrickIndex]->Destroy();
			ElemBricks[elemBrickIndex] = NULL;
		}
	}

	if (horizontalSeq.Num() >= 4)
	{
		for (int i = 0; i < horizontalSeq.Num(); i++)
		{
			int32 elemBrickIndex = horizontalSeq[i]->CubeIndex;
			ElemBricks[elemBrickIndex]->Destroy();
			ElemBricks[elemBrickIndex] = NULL;
		}
	}
}