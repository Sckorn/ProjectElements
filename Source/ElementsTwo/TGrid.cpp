// Fill out your copyright notice in the Description page of Project Settings.

#include "ElementsTwo.h"
#include "TGrid.h"
#include "BaseElementCube.h"

template <typename T>
TGrid<T>::TGrid()
{
}

template <typename T>
TGrid<T>::TGrid(uint32 initialSize)
{
	HoldingArray = new T[initialSize];
}

template <typename T>
TGrid<T>::TGrid(uint32 initialSize, pfn _callback)
{
	HoldingArray = new T[initialSize];
	Callback = _callback;
}

template <typename T>
TGrid<T>::~TGrid()
{
	delete[] HoldingArray;
}
