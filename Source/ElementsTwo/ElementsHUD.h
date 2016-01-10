// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "ElementsHUD.generated.h"

/**
 * 
 */
UCLASS()
class ELEMENTSTWO_API AElementsHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
		UFont * HUDFont;

	AElementsHUD(const FObjectInitializer& ObjectInitializer);
	
	virtual void DrawHUD() override;
};
