// Fill out your copyright notice in the Description page of Project Settings.

#include "ElementsTwo.h"
#include "ElectricElementCube.h"

AElectricElementCube::AElectricElementCube() : Super()
{
	static ConstructorHelpers::FObjectFinder<UMaterial> MyMaterial(TEXT("Material'/Game/StarterContent/Materials/M_Basic_Electricity_element_Moving.M_Basic_Electricity_element_Moving'"));
	if (MyMaterial.Succeeded())
	{
		VisibleCompponent->SetMaterial(0, MyMaterial.Object);
	}

	BrickType = EElementType::EElectricity;
}

/*void AElectricElementCube::ClickHandler(UPrimitiveComponent* ClickedComp, AElementsTwoGameMode * MyGameMode)
{
	Super::ClickHandler(ClickedComp, MyGameMode);
	TArray<AActor*> CollectedActors;
	SphereComponent->GetOverlappingActors(CollectedActors);	
	for (int32 i = 0; i < CollectedActors.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlapping with: %s"), *CollectedActors[i]->GetName());
	}
}*/


