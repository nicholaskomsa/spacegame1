#include <include/wrapper/cef_helpers.h>
#include <include/wrapper/cef_stream_resource_handler.h>
#include <include/cef_app.h>
#pragma once



class ClientResourceHandler : public CefResourceHandler {
public:
	ClientResourceHandler();
	std::string getFileName(std::string url);

	virtual bool ProcessRequest(CefRefPtr<CefRequest> request,
		CefRefPtr<CefCallback> callback)
		OVERRIDE;

	virtual void GetResponseHeaders(CefRefPtr<CefResponse> response,
		int64& response_length,
		CefString& redirectUrl) OVERRIDE;

	virtual void Cancel() OVERRIDE;

	virtual bool ReadResponse(void* data_out,
		int bytes_to_read,
		int& bytes_read,
		CefRefPtr<CefCallback> callback)
		OVERRIDE;
private:
	std::string mData;
	std::string mMimeType;
	size_t mOffset;

	IMPLEMENT_REFCOUNTING(ClientResourceHandler);
};

class ClientSchemeHandlerFactory : public CefSchemeHandlerFactory {
public:

	virtual CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		const CefString& scheme_name,
		CefRefPtr<CefRequest> request)
		OVERRIDE;

	IMPLEMENT_REFCOUNTING(ClientSchemeHandlerFactory);
};
