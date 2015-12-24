// Fill out your copyright notice in the Description page of Project Settings.

#include "ElementsTwo.h"
#include "ElementsPlayerController.h"

AElementsPlayerController::AElementsPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}


