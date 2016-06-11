// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseElementCube.h"
#include "GameFramework/GameMode.h"
#include "ElementsTwoGameMode.generated.h"

/**
 * 
 */

UCLASS()
class ELEMENTSTWO_API AElementsTwoGameMode : public AGameMode
{
	GENERATED_BODY()

private:

	float alpha;
	FVector FirstCubeInitialPosition;
	FVector SecondCubeInitialPosition;
	bool bGameActive;
	FTimerHandle fTimerHandle;
	FTimerHandle fPauseTimerHandle;
	FVector targetPosition;
	
public:

	AElementsTwoGameMode();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EDitAnywhere)
		int32 totalBricksToSpawn;

	UPROPERTY(EditAnywhere)
		int32 bricksInARow;

	UPROPERTY()
		int32 totalBrickTypes;

	UPROPERTY(EditAnywhere)
		ABaseElementCube * SelectedCube;

	UPROPERTY(EditAnywhere)
		ABaseElementCube * SecondSelectedCube;
	
	UPROPERTY(EditAnywhere)
		TArray<ABaseElementCube *> ElemBricks;

	UPROPERTY(EditAnywhere)
		TArray<int32> CubeIndexesToDeleteHorizontal;

	UPROPERTY(EditAnywhere)
		TArray<int32> CubeIndexesToDeleteVertical;

	UPROPERTY(EditAnywhere)
		TArray<int32> EmptyIndexes;

	UPROPERTY(EditAnywhere)
		bool bInputEnabled;

	UPROPERTY()
		bool bSwapStarted;

	UPROPERTY()
		bool bDeleteSequenceStarted;

	UPROPERTY()
		int32 highScore;

	UPROPERTY()
		int32 iCountdownTime;

	UPROPERTY(EditAnywhere)
		int32 iPauseCountdownTime;

	UPROPERTY()
		int32 currentDeletingIndex;

	UPROPERTY()
		int32 tmpIndex;

	UPROPERTY()
		int32 lowestIndex;

	UPROPERTY()
		int32 startedIndex;

	UPROPERTY()
		int32 verticalSequenceAmount;

	UPROPERTY()
		float moveDownAlpha;

	UPROPERTY()
		ABaseElementCube * pDelete;

	UFUNCTION()
		void SwapCubesInit(ABaseElementCube * SecondSelected);

	UFUNCTION()
		EElementType DetectFourInARow(int32 index, EElementType type);

	UFUNCTION()
		EElementType GetNextAvailableType(EElementType currentType);

	UFUNCTION()
		void SwapCoordinates();

	UFUNCTION()
		void FindFourInARow(int32 index);

	UFUNCTION()
		void DeleteCubes(const TArray<int32> toDelete);

	UFUNCTION()
		void DeleteCubesLerpMode(const TArray<int32> toDelete);

	UFUNCTION()
		void StartDeletingSequence(const TArray<int32> toDelete);

	UFUNCTION()
		void AfterCubesDelete(const TArray<int32> removedIndexes);

	UFUNCTION()
		void AdvanceTimer();

	UFUNCTION()
		void AdvancePauseTimer();

	UFUNCTION()
		void OnPauseCountdownEnd();

	UFUNCTION()
		void OnCountdownEnd();

	UFUNCTION()
		void AddToTimer(int32 iTimeToAdd);

	UFUNCTION()
		void PauseMainTimer();

	UFUNCTION()
		void ResumeMainTimer();

	UFUNCTION()
		void AfterDeleteHorizontalSpawner(TArray<int32> deletedIndexes);

	UFUNCTION()
		void SpawnCubeAtPosition(int32 indexToSpawnAt);

	UFUNCTION()
		void SequenceDetector(ABaseElementCube * selCb);

	/*UFUNCTION()
		void PerformSpecialAction();*/
};
