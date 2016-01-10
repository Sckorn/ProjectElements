// Fill out your copyright notice in the Description page of Project Settings.

#include "ElementsTwo.h"
#include "ElementsHUD.h"
#include "ElementsTwoGameMode.h"

AElementsHUD::AElementsHUD(const FObjectInitializer& ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UFont> HUDFontObj(TEXT("/Engine/EngineFonts/RobotoDistanceField"));
	if (HUDFontObj.Object != NULL)
	{
		HUDFont = HUDFontObj.Object;
	}
}

void AElementsHUD::DrawHUD()
{
	FVector2D ScreenDimensions = FVector2D(Canvas->SizeX, Canvas->SizeY);
	Super::DrawHUD();
	AElementsTwoGameMode * CurrentGameMode = Cast<AElementsTwoGameMode>(UGameplayStatics::GetGameMode(this));
	FString HighscoreString = FString::Printf(TEXT("%d"), CurrentGameMode->highScore);
	DrawText(HighscoreString, FColor::White, 50, 50, HUDFont);
	FString sTimeString = FString::Printf(TEXT("%d:%d"), (CurrentGameMode->iCountdownTime / 60), (CurrentGameMode->iCountdownTime % 60));
	DrawText(sTimeString, FColor::White, (Canvas->SizeX - 100), 50, HUDFont);
}


