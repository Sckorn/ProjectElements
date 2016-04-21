// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <fstream>

/**
 * 
 */
class ELEMENTSTWO_API Logger
{
private:
	static char * fileName;
	static char * filePath;
	static std::ofstream * fileStreamHandle;
public:
	/*Logger();
	~Logger();*/

	static void CreateLogFile();
	static bool CheckFileExists();
	static void Dispose();
	static void SetLogFilePath(char * filePath);
	static void SetFileName(char * fileName);
	static void WriteToLog(const char * stringToWrite);
};
