
#include "log.h"


Log::End Log::end;

Log::Log(std::string filename, std::_Iosb<int>::_Openmode openMode)
	:mFileName(filename)
{
	std::ofstream fout;
	fout.open(mFileName, openMode);
	if (fout.fail())
		EXCEPT << "Log::Log, failed to open file: " << filename << "\n";
	fout.close();
}
Log::~Log() {
	*this << end;
}
std::map< std::string, Log > LogManager::mLogs;
LogManager LogManager::makeInitialLog;

LogManager::LogManager() {
	add("game", "game.log", std::ios::out);
}

void LogManager::add(std::string name, std::string fileName, std::_Iosb<int>::_Openmode openMode) {

	bool alreadyExists = false;
	if (mLogs.find(name) != mLogs.end())
		alreadyExists = true;
	//	EXCEPT << "LogManager::add, log, " << name << " already exists!";

	mLogs[name] = Log(fileName, openMode);
	if (alreadyExists)
		mLogs[name] << "Replaced a previous log with same name" << Log::end;
}
Log& LogManager::get(std::string name) {
	auto log = mLogs.find(name);
	if (log == mLogs.end())
		EXCEPT << "LogManager::get, log, " << name << " not found!";

	return log->second;
}
