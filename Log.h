/*

	Log.h

*/
#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

class Log
{
public:
	static void Log::AppendToLogFile(std::string text);
	static void Log::AppendToLogFileWithDate(std::string text);
};

