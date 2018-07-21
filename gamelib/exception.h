
#include <sstream>

#pragma once


struct Exception {
	std::string message{ "Exception" };

	Exception(std::string file, int line);

	template<typename T>
	Exception& operator<<(T value) {
		std::stringstream sstr;
		sstr << message << value;
		message = sstr.str();
		return *this;
	}

	void writeDebugString();
};

#define EXCEPT throw Exception( __FILE__, __LINE__ )
