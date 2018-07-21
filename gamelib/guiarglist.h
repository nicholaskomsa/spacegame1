
#include <string>
#include <vector>
#include <iosfwd>
#include <sstream>

#pragma once


class GuiArgList {
	typedef std::vector< std::string > JSGuiArgList;
	JSGuiArgList jsGuiArgList;
	std::string curPos;
public:
	//std::stringstream toStream();
	void push(std::string s);

	GuiArgList& operator[](int index);

	template<typename T>
	operator T() {
		std::stringstream sstr;
		sstr << curPos;
		T t;
		sstr >> t;
		return t;
	}

	std::size_t size();
	std::string toString();
};
