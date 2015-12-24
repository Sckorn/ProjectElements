// Fill out your copyright notice in the Description page of Project Settings.

#include "ElementsTwo.h"
#include "AcidElementCube.h"

AAcidElementCube::AAcidElementCube() : Super()
{
	static ConstructorHelpers::FObjectFinder<UMaterial> MyMaterial(TEXT("Material'/Game/StarterContent/Materials/M_Acid.M_Acid'"));
	if (MyMaterial.Succeeded())
	{
		VisibleCompponent->SetMaterial(0, MyMaterial.Object);
	}

	BrickType = EElementType::EAcid;
}


