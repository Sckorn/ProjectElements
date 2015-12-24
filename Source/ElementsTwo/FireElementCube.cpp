// Fill out your copyright notice in the Description page of Project Settings.

#include "ElementsTwo.h"
#include "FireElementCube.h"

AFireElementCube::AFireElementCube() : Super()
{
	static ConstructorHelpers::FObjectFinder<UMaterial> MyMaterial(TEXT("Material'/Game/StarterContent/Materials/M_Fire.M_Fire'"));
	if (MyMaterial.Succeeded())
	{
		VisibleCompponent->SetMaterial(0, MyMaterial.Object);
	}

	BrickType = EElementType::EFire;
}


