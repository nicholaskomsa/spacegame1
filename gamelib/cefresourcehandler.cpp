#include "cefresourcehandler.h"

#include <boost/algorithm/string/find.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <fstream>


ClientResourceHandler::ClientResourceHandler()
	: mOffset(0)
{}

std::string  ClientResourceHandler::getFileName(std::string url) {
	//get rid of scheme and domain by locating the third '/', leaves with file path + name
	boost::iterator_range<std::string::iterator> r = boost::find_nth(url, "/", 3 - 1);
	boost::iterator_range<std::string::iterator> rtoerase(url.begin(), r.end());

	boost::erase_range(url, rtoerase);
	return url;
}

bool  ClientResourceHandler::ProcessRequest(CefRefPtr<CefRequest> request,
	CefRefPtr<CefCallback> callback) {

	CEF_REQUIRE_IO_THREAD();

	bool handled = false;

	std::string url = request->GetURL();
	std::string fileName = getFileName(url);
	if (fileName == "") return true; // when browser sync created, just domain passed in so filename == "", skip routine

	if (url.find(".js") != std::string::npos) {
		mMimeType = "application/javascript"; handled = true;
	}
	if (url.find(".css") != std::string::npos) {
		mMimeType = "text/css";	handled = true;		//CHECK THIS IS CORRECT
	}
	if (url.find(".html") != std::string::npos || url.find(".htm") != std::string::npos) {
		mMimeType = "text/html"; handled = true;
	}

	//add cases to allow for specific kinds of resources, such as jpg and png
	//			mime_type_ = "image/png";
	if (url.find(".jpg") != std::string::npos) {
		mMimeType = "image/jpeg"; handled = true;
	}
	if (url.find(".png") != std::string::npos) {
		mMimeType = "image/png"; handled = true;
	}
	if (url.find(".gif") != std::string::npos) {
		mMimeType = "image/gif"; handled = true;
	}

	if (handled) {
		std::ifstream fin;

		fin.open(fileName, std::ios::ate | std::ios::binary);
		if (fin.fail()) return false;

		std::streamoff filesize = fin.tellg();	//get the length of the file by doing ate
		fin.seekg(0);					//change position back to 0 for reading

		char* buffer = new char[filesize];
		fin.read(buffer, filesize);

		fin.close();

		mData.resize(filesize);
		memcpy(const_cast<char*>(mData.c_str()), buffer, filesize);

	}
	else {

		std::stringstream sstr;
		sstr << "SCHEME unhandled " << url << " , ";
		OutputDebugStringA(sstr.str().c_str());
	}

	if (handled) {
		// Indicate the headers are available.
		callback->Continue();
		return true;
	}

	return false;
}

void  ClientResourceHandler::GetResponseHeaders(CefRefPtr<CefResponse> response,
	int64& response_length,
	CefString& redirectUrl) {

	CEF_REQUIRE_IO_THREAD();

	DCHECK(!mData.empty());
	OutputDebugStringA("RESPONSE HEADER");
	response->SetMimeType(mMimeType);
	response->SetStatus(200);

	// Set the resulting response length
	response_length = mData.length();
}

void ClientResourceHandler::Cancel() {

	CEF_REQUIRE_IO_THREAD();
}

bool  ClientResourceHandler::ReadResponse(void* data_out,
	int bytes_to_read,
	int& bytes_read,
	CefRefPtr<CefCallback> callback)
{

	CEF_REQUIRE_IO_THREAD();

	bool has_data = false;
	bytes_read = 0;
	OutputDebugStringA("RESPONSE READ");
	if (mOffset < mData.length()) {
		// Copy the next block of data into the buffer.
		int transfer_size =
			min(bytes_to_read, static_cast<int>(mData.length() - mOffset));
		memcpy(data_out, mData.c_str() + mOffset, transfer_size);
		mOffset += transfer_size;

		bytes_read = transfer_size;
		has_data = true;
	}
	OutputDebugStringA("RESPONSE DONE");
	return has_data;
}


CefRefPtr<CefResourceHandler> ClientSchemeHandlerFactory::Create(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	const CefString& scheme_name,
	CefRefPtr<CefRequest> request)
{
	// Return a new resource handler instance to handle the request.
	return new ClientResourceHandler();
}

