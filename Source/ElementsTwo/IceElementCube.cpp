// Fill out your copyright notice in the Description page of Project Settings.

#include "ElementsTwo.h"
#include "IceElementCube.h"

AIceElementCube::AIceElementCube() : Super()
{
	static ConstructorHelpers::FObjectFinder<UMaterial> MyMaterial(TEXT("Material'/Game/StarterContent/Materials/M_Basic_Ice_Noopacity.M_Basic_Ice_Noopacity'"));
	if (MyMaterial.Succeeded())
	{
		VisibleCompponent->SetMaterial(0, MyMaterial.Object);
	}

	BrickType = EElementType::EIce;
}


