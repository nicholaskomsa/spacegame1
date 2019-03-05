

#include "graphics.h"
#include <include/cef_render_handler.h>



#pragma once




class CefRenderHandlerOgre : public CefRenderHandler {
public:
	Ogre::TexturePtr renderTexture;
	Ogre::SceneNode* renderNode;
	Ogre::SceneManager* pSceneMgr;
	const std::string sceneNodeName;
	const std::string geoName;
	const std::string textureName;
	const std::string materialName;

	CefRenderHandlerOgre(Ogre::SceneManager* pSceneMgr1);
	~CefRenderHandlerOgre();


	// CefCefRenderHandlerOgre interface
	void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect);
	void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height);
	void clear();
	void resize(int width, int height);

public:

public:
	IMPLEMENT_REFCOUNTING(CefRenderHandlerOgre);
};

