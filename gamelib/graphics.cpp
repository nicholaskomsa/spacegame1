
#include <SDL_syswm.h>
#include <SDL_video.h>

#include <OgreConfigDialog.h>

#include "graphics.h"


void Graphics::setup(Ogre::String windowTitle, Ogre::String sceneManagerType, Ogre::String resourcesFileName, Ogre::String pluginsFileName, Ogre::String cfgFileName, Ogre::String logFileName) {

	if (mRoot) EXCEPT << "graphics already setup";

	mRoot = OGRE_NEW Ogre::Root(pluginsFileName, cfgFileName, logFileName);

//	Ogre::ConfigDialog dialog;

	if (mRoot->restoreConfig() ) { // || mRoot->showConfigDialog(&dialog)

		mRoot->initialise(false);

		Ogre::NameValuePairList miscParams;
		Ogre::ConfigOptionMap ropts = mRoot->getRenderSystem()->getConfigOptions();

		if (miscParams.empty())
		{
			miscParams["FSAA"] = ropts["FSAA"].currentValue;
			miscParams["vsync"] = ropts["VSync"].currentValue;
		}

		miscParams["Full Screen"] = ropts["Full Screen"].currentValue;


		bool fullScreen = miscParams["Full Screen"] == "Yes";

		int w = 800, h = 600;

		//set the window size to the selected video mode, unsure if generally sensical
		std::istringstream mode(ropts["Video Mode"].currentValue);
		Ogre::String token;
		mode >> w; // width
		mode >> token; // 'x' as seperator between width and height
		mode >> h; // height


		setupSDLVideo(windowTitle, fullScreen, w, h);

		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		SDL_GetWindowWMInfo(mSDLWindow, &wmInfo);

		miscParams["externalWindowHandle"] = Ogre::StringConverter::toString( std::size_t( wmInfo.info.win.window));

		std::string windowName = "OgreRenderWindow";
		mWindow = mRoot->createRenderWindow( windowName, w, h, fullScreen, &miscParams);


	}
	else {
		Ogre::LogManager::getSingleton().getDefaultLog()->logMessage("Graphics::Cancel Button Hit!");
		return;
	}

	mSceneManager = mRoot->createSceneManager(sceneManagerType, "DefaultSceneManager");

	//if (!initShaderGenerator()) {

	//	EXCEPT << "Graphics::create initShaderGenerator Failed!";
	//}


	addResourceLocations(resourcesFileName);

	createCameraAndViewport();

	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
bool Graphics::initShaderGenerator() {
	if (Ogre::RTShader::ShaderGenerator::initialize())
	{
		// Grab the shader generator pointer.
		mShaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
		// Add the shader libs resource location. a sample shader lib can be found in Samples\Media\RTShaderLib
		//Ogre::ResourceGroupManager::getSingleton().addResourceLocation(shaderLibPath, "FileSystem");	-this is added manually in resources.cfg
		// Set shader cache path.
	//	mShaderGenerator->setShaderCachePath(shaderCachePath);
		// Set the scene manager.
		mShaderGenerator->addSceneManager(mSceneManager);
		return true;
	}
	return false;
}
void Graphics::shutdown() {
	if( mShaderGenerator) mShaderGenerator->removeSceneManager(mSceneManager);

	if (mRoot) {
		mRoot->destroySceneManager(mSceneManager);

		OGRE_DELETE mRoot;
		mRoot = NULL;

	}
	if (mSDLWindow) SDL_DestroyWindow(mSDLWindow);
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void Graphics::go() {
	mRoot->startRendering();
}

void Graphics::setupSDLVideo(std::string windowTitle, bool fullScreen, int width, int height) {

	if (!SDL_WasInit(SDL_INIT_VIDEO)) {
		SDL_InitSubSystem(SDL_INIT_VIDEO);
	}
	SDL_WindowFlags flags = fullScreen? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_RESIZABLE;

	Uint32 f = flags ;

	mSDLWindow = SDL_CreateWindow(windowTitle.c_str(),
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, f );
}

void Graphics::addResourceLocations(std::string resourcesFileName) {
	Ogre::ConfigFile config;

	config.load(resourcesFileName);

	//	Ogre::ConfigFile::SectionIterator seci = config.getSectionIterator();

	const Ogre::ConfigFile::SettingsBySection_& settingsBySection = config.getSettingsBySection();

	for (auto section = settingsBySection.begin(); section != settingsBySection.end(); section++) {
		auto settings = section->second;
		for (auto setting = settings.begin(); setting != settings.end(); setting++) {

			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				setting->second, setting->first, section->first);
		}
	}

	/*
	Ogre::String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
	secName = seci.peekNextKey();
	Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
	Ogre::ConfigFile::SettingsMultiMap::iterator i;
	for (i = settings->begin(); i != settings->end(); ++i)
	{
	typeName = i->first;
	archName = i->second;
	#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE || OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
	// OS X does not set the working directory relative to the app,
	// In order to make things portable on OS X we need to provide
	// the loading with it's own bundle path location
	if (!StringUtil::startsWith(archName, "/", false)) // only adjust relative dirs
	archName = Ogre::String(macBundlePath() + "/" + archName);
	#endif
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
	archName, typeName, secName);

	}
	}
	*/
}

void Graphics::createCameraAndViewport(Ogre::ColourValue backgroundColor) {

	if (!mSceneManager) {
		EXCEPT << "Graphics::createCameraAndViewport sceneManager=NULL";
	}
	
	if( !mCamera )
		mCamera = mSceneManager->createCamera("DefaultCamera");

	if (!mCamera) {
		EXCEPT << "Graphics::createCameraAndViewport, createCamera Failed, camera=NULL";
	}
	// Create one viewport, entire window
	
	mWindow->removeAllViewports();
	Ogre::Viewport* vp = mWindow->addViewport(mCamera);
	if (!vp) {
		EXCEPT << "Graphics::createCameraAndViewport, addViewport Failed, vp=NULL";
	}

	vp->setBackgroundColour(backgroundColor);

	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio(
		Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

	mCamera->setNearClipDistance(1);

	vp->setCamera(mCamera); 
}

void Graphics::setFrameListener( Ogre::FrameListener* frameListener) {
	mFrameListener = frameListener;

	if (mFrameListener) mRoot->addFrameListener(mFrameListener);
}

Ogre::SceneManager* Graphics::getSceneManager() {
	return mSceneManager;
}
Ogre::RenderWindow* Graphics::getRenderWindow() {
	return mWindow;
}
SDL_Window* Graphics::getSDLWindow() {
	return mSDLWindow;
}
Ogre::Camera* Graphics::getCamera() {
	return mCamera;
}
Ogre::Root* Graphics::getRoot() {
	return mRoot;
}

Ogre::RTShader::ShaderGenerator* Graphics::getShaderGenerator() { return mShaderGenerator; }
