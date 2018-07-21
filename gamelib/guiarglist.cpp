#include "guiarglist.h"


/*
std::stringstream GuiArgList::toStream(){
std::stringstream sstr;
for( std::size_t i =0; i < jsGuiArgList.size(); i++)
sstr << jsGuiArgList[i];
return sstr;
}

*/

void GuiArgList::push(std::string s) {
	jsGuiArgList.push_back(s);
}


//template<typename T>
GuiArgList& GuiArgList::operator[](int index) {
	curPos = jsGuiArgList[index];
	return *this;
}



std::size_t GuiArgList::size() {
	return jsGuiArgList.size();
}
std::string GuiArgList::toString() {
	return curPos;
}


