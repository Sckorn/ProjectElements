// Fill out your copyright notice in the Description page of Project Settings.

#include "ElementsTwo.h"
#include "Logger.h"
#include <fstream>
#include <string.h>

char * Logger::fileName;
char * Logger::filePath;
std::ofstream * Logger::fileStreamHandle;

/*Logger::Logger()
{

	//std::ofstream logFileStream();
}

Logger::~Logger()
{
}*/

void Logger::CreateLogFile()
{
	Logger::fileStreamHandle = new std::ofstream(strcat(Logger::filePath, Logger::fileName));
}

bool Logger::CheckFileExists()
{
	

	return true;
}

void Logger::Dispose()
{
	Logger::fileStreamHandle->close();
	delete Logger::fileStreamHandle;
}

void Logger::SetFileName(char * fileName)
{
	Logger::fileName = fileName;
}

void Logger::SetLogFilePath(char * filePath)
{
	Logger::filePath = filePath;
}

void Logger::WriteToLog(const char * stringToWrite)
{
	if (Logger::fileStreamHandle->good())
	{
		Logger::fileStreamHandle->write(stringToWrite, strlen(stringToWrite));
	}
}
