// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 * 
 */
/*USTRUCT()
struct FSingleElementEngineDimensions
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY()
		int32 SampleInt32;
};*/

typedef void(*pfn)();

template <typename T>
class ELEMENTSTWO_API TGrid
{
private:
	T * HoldingArray = new T[1];
	pfn Callback;
	float startPositionX;
	float startPositionY;


public:
	TGrid();
	TGrid(uint32 initialSize);
	TGrid(uint32 initialSize, pfn _callback);
	~TGrid();
};
