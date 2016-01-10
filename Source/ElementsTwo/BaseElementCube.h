// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "BaseElementCube.generated.h"

UENUM()
enum class EElementType : uint8
{
	EIce,
	EFire,
	EElectricity,
	EAcid
};

UCLASS()
class ELEMENTSTWO_API ABaseElementCube : public AActor
{
	GENERATED_BODY()

private:

	bool bSelected;
	float alpha;
	FVector UpLocation;
	FVector DownLocation;
	FVector FromLocation;
	int direction;
	int32 differenceBetweenCenters;
	
public:	
	// Sets default values for this actor's properties
	ABaseElementCube();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent * VisibleCompponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseElement)
		USphereComponent * SphereComponent;

	UPROPERTY()
		EElementType BrickType;

	UFUNCTION()
		void OnClicked(UPrimitiveComponent* ClickedComp);
	
	UFUNCTION()
		virtual void ClickHandler(UPrimitiveComponent* ClickedComp, AElementsTwoGameMode * OurGameMode);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BaseElement)
		FVector InitialPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CubeCoordinates)
		int32 CubeX;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CubeCoordinates)
		int32 CubeY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CubeCoordinates)
		int32 CubeIndex;

	UPROPERTY()
		int32 scoreValue;

	UPROPERTY()
		int32 iSpecialActionModifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DebugInfo)
		bool bDestroyed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DebugInfo)
		bool bShowDebugInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DebugInfo)
		bool bAfterDestroyGenerated;

	UFUNCTION()
		void SelfToInitialPosition();

	UFUNCTION()
		void StopHighlighting();

	UFUNCTION()
		virtual void OnSwapEnd();

	UFUNCTION()
		virtual void FourInARow();

	UFUNCTION()
		virtual void MoveDown(int32 multiplier = 1);

	UFUNCTION()
		virtual void SpecialAction();

	UFUNCTION()
		void SetSpecialActionModifier(int32 iModifier);
};
