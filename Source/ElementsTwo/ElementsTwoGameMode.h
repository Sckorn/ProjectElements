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
		bool bInputEnabled;

	UPROPERTY()
		bool bSwapStarted;

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
};
