

#include <include/cef_app.h>


#pragma once



// Client app implementation for other process types.
class CefAppOther : public CefApp {
public:
	CefAppOther() {}

private:
	IMPLEMENT_REFCOUNTING(CefAppOther);
	DISALLOW_COPY_AND_ASSIGN(CefAppOther);
};