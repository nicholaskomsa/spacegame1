#include "guisheetmanagerrenderer.h"
#include "guisheetmanagermessages.h"
#include "log.h"

GuiSheetManagerRenderer::GuiSheetManagerRenderer()
{}


void GuiSheetManagerRenderer::setRendererGui(CefRefPtr<CefProcessMessage> msg, CefRefPtr<CefBrowser> browser1) {
	if (msg->GetName() != GuiSheetManagerMessages::msgSetBindings ) return;

	CefRefPtr<CefListValue> msgArgs = msg->GetArgumentList();

	rbindings.clear();
	for (int binding = 0; binding < msgArgs->GetSize() - 1; binding++) {
		rbindings[msgArgs->GetString(binding)] = RBinding();
	}
	//last arg is url to set
	
	//->GetMainFrame()->

	LogManager::get("renderer") << "LOAD URL " << msgArgs->GetString(static_cast<int>(msgArgs->GetSize()) - 1)  << " curent: "  << Log::end ;

	browser1->GetMainFrame()->LoadURL(msgArgs->GetString(static_cast<int>(msgArgs->GetSize()) - 1));
}

void GuiSheetManagerRenderer::generateV8Bindings(CefRefPtr<CefV8Value> object, CefRefPtr<CefV8Handler> handler) {
	LogManager::get("renderer") << "generate V8 bindings" << Log::end;

	for (RendererBindingsMap::iterator it = rbindings.begin(); it != rbindings.end(); it++) {

		std::string funcName = it->first;
		LogManager::get("renderer") << "binding: " << funcName << Log::end;

		CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction(funcName, handler);

		LogManager::get("renderer") << "func: " << (func != NULL) << "; obj: " << (object != NULL ) <<Log::end;

		object->SetValue(funcName, func, V8_PROPERTY_ATTRIBUTE_NONE);
		
	}
}

bool GuiSheetManagerRenderer::sendJavascriptToBrowser(CefRefPtr<CefBrowser> browser1, std::string javascriptID, const CefV8ValueList& args) {
	std::string messageID = javascriptID;
	LogManager::get("renderer") << "send javascript to browser ID=" << messageID << Log::end;
	if (rbindings.find(messageID) != rbindings.end()) {
		CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create(messageID);
		CefRefPtr<CefListValue> msgArgs = msg->GetArgumentList();

		//pack all the args as strings
		for (std::size_t i = 0; i < args.size(); i++) {
			std::stringstream sstr;

			if (args[i]->IsInt())
				sstr << args[i]->GetIntValue();
			else if (args[i]->IsString())
				sstr << std::string(args[i]->GetStringValue());
			else if (args[i]->IsDouble())
				sstr << args[i]->GetDoubleValue();
			else if (args[i]->IsBool())
				sstr << args[i]->GetBoolValue();
			else if (args[i]->IsUInt())
				sstr << args[i]->GetUIntValue();
			else
				sstr << "UNDEFINED CONVERSION";
			msgArgs->SetString(i, sstr.str());
		}

		
		browser1->SendProcessMessage(PID_BROWSER, msg);
		return true;
	}
	return false;
}