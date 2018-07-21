#pragma once
#include <fstream>
#include <map>

#include "exception.h"

#pragma once



class Log {
	std::string mFileName;
	std::string message;
public:

	Log(std::string filename = "game.log", std::_Iosb<int>::_Openmode openMode = std::ios::out);
	~Log();

	template<typename T>
	Log& operator<<(T t) {
		std::stringstream sstr;
		sstr << message << t;
		message = sstr.str();
		return *this;
	}

	struct End {};
	static End end;

	Log& operator<<(End e) {
		std::ofstream fout;
		fout.open(mFileName, std::ios::app);
		if (fout.fail())
			EXCEPT << "Log::<<end open failed! " << mFileName;
		fout << message << "\n";
		fout.close();
		message.clear();
		return *this;
	}
};

class LogManager {
	static std::map< std::string, Log > mLogs;

	LogManager();
	static LogManager makeInitialLog;

public:

	static void add(std::string name, std::string fileName, std::_Iosb<int>::_Openmode openMode = std::ios::app);
	static Log& get(std::string name);
};