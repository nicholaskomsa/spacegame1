
#include "cefrenderhandlerOgre.h"

#include <memory>


CefRenderHandlerOgre::CefRenderHandlerOgre(Ogre::SceneManager* pSceneMgr1)
	: pSceneMgr(pSceneMgr1)
	, sceneNodeName("CefGuiBrowserRendererSceneNode")
	, geoName("CefGuiBrowserRendererGeo")
	, textureName("CefGuiBrowserRendererTexture")
	, materialName("CefGuiBrowserRendererMaterialName") //materialName("CefGuiBrowserRendererMaterial")
	, renderNode(NULL)
{}
CefRenderHandlerOgre::~CefRenderHandlerOgre() {

	//clear();
}
void CefRenderHandlerOgre::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect)
{
	if (!renderTexture)
		rect = CefRect(0, 0,10, 10);
	else
		rect = CefRect(0, 0, renderTexture->getWidth(), renderTexture->getHeight());
}
void CefRenderHandlerOgre::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height)
{

	//FIX THESE MULTIPLIES
	if (!renderTexture ) return;
	int size = width*height;
	const int sizeOfWord = 4;
	
	if (size != renderTexture->getWidth() * renderTexture->getHeight()) return;

	int w = renderTexture->getWidth();
	int h = renderTexture->getHeight();



	/*
	std::vector< Uint32 > image;
	image.resize(width* height);
	std::size_t p = 0;
	for (std::size_t x = 0; x < width; x++) {
		for (std::size_t y = 0; y < height; y++) {
			p = (y*width) + x;

			unsigned char r = 0;
			y % 2  ==0 || y%3 ==0 || y%4 ==0 ? r = 255 : r = 0;

			((char*)&image[p])[0] = 0;
			((char*)&image[p])[1] = 0;
			((char*)&image[p])[2] = r;
			((char*)&image[p])[3] = 255;
		}
			
	}
	std::size_t endP = p - (size-1);
	*/

	int d = dirtyRects.size();
	CefRect r = dirtyRects[0];

	Ogre::HardwarePixelBufferSharedPtr texBuf = renderTexture->getBuffer();
	texBuf->lock(Ogre::HardwareBuffer::HBL_DISCARD);

	memcpy(texBuf->getCurrentLock().data, buffer, size * sizeOfWord);

	texBuf->unlock();
	
}
void CefRenderHandlerOgre::clear() {
	renderNode = NULL;
	if (pSceneMgr) {
		if (pSceneMgr->hasSceneNode(sceneNodeName)) pSceneMgr->destroySceneNode(sceneNodeName);
		if (pSceneMgr->hasManualObject(geoName)) pSceneMgr->destroyManualObject(geoName);
	}
	if (Ogre::MaterialManager::getSingletonPtr()->resourceExists(materialName)) Ogre::MaterialManager::getSingletonPtr()->remove(materialName);
	if (Ogre::TextureManager::getSingleton().resourceExists(textureName)) Ogre::TextureManager::getSingleton().remove(textureName);
	renderTexture.reset();
}

void CefRenderHandlerOgre::resize(int width, int height) {
	if (!pSceneMgr) return;
	//if (renderTexture && (renderTexture->getWidth() == width && renderTexture->getHeight() == height )) return;

	//first clear everything that may exist
	clear();

	
	renderTexture = Ogre::TextureManager::getSingleton().createManual(
		textureName,
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D, width, height, 0, Ogre::PF_A8R8G8B8, Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

	int sw =	renderTexture->getSrcWidth();
	int sh = renderTexture->getSrcHeight();
	int w = renderTexture->getWidth();
	int h = renderTexture->getHeight();


	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingletonPtr()->create(materialName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	//material->getTechnique(0)->getPass(0)->setCullingMode(Ogre::CULL_NONE); // print both sides of the polygones
	material->getTechnique(0)->getPass(0)->createTextureUnitState(textureName);
	material->getTechnique(0)->getPass(0)->setLightingEnabled(false);
	
	material->setDepthWriteEnabled( false );
	Ogre::PixelFormat pf = renderTexture->getFormat();
	material->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SceneBlendType::SBT_TRANSPARENT_ALPHA);
	

	Ogre::ManualObject* geo = pSceneMgr->createManualObject(geoName);
	geo->setUseIdentityProjection(true);
	geo->setUseIdentityView(true);
	geo->setDynamic(true);

	// Render just before overlays
	geo->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);
	
	geo->begin(materialName);

	Ogre::Real x = 0, y = 0;
	int offset = 1;
	/*
	(1) 0, 1			(3) 1, 1
	(0) 0, 0			(2) 1, 0

	0, 2, 1
	2, 3, 1
	*/

	geo->position(Ogre::Vector3(x - offset, y - offset, 0));  geo->textureCoord(0, 1);  
	geo->position(Ogre::Vector3(x - offset, y + offset, 0));  geo->textureCoord(0, 0);
	geo->position(Ogre::Vector3(x + offset, y - offset, 0));  geo->textureCoord(1, 1);
	geo->position(Ogre::Vector3(x + offset, y + offset, 0));  geo->textureCoord(1, 0);

	geo->triangle(0, 2, 1);
	geo->triangle(2, 3, 1);

	geo->end();

	Ogre::AxisAlignedBox aabInf;
	aabInf.setInfinite();
	geo->setBoundingBox(aabInf);


	renderNode = pSceneMgr->getRootSceneNode()->createChildSceneNode(sceneNodeName, Ogre::Vector3(0., 0, 0));

	renderNode->attachObject(geo);
	//renderNode->setVisible(false);
}
