#include <ogre.h>

#include <memory>

#include <SDL.h>


#include "exception.h"
#include "log.h"


#pragma once


class Graphics {
private:

	SDL_Window* mSDLWindow{ nullptr };

	Ogre::Root* mRoot{ nullptr };

	Ogre::RenderWindow* mWindow{ nullptr };
	Ogre::SceneManager* mSceneManager{ nullptr };
	Ogre::Camera* mCamera{ nullptr } ;



	void addResourceLocations(std::string resourcesFileName);

	Ogre::FrameListener* mFrameListener;
	//Ogre::WindowEventListener* mWindowEventListener;//no longer used

	void setupSDLVideo(std::string windowName, bool fullScreen, int width = 800, int height = 600);

public:

	void setup( Ogre::String windowTitle = "Ogre Render Window", Ogre::String sceneManagerType = "DefaultSceneManager", Ogre::String resourcesFileName = "resources.cfg", Ogre::String pluginsFileName = "plugins.cfg", Ogre::String cfgFileName = "ogre.cfg", Ogre::String logFileName = "ogre.log");
	void shutdown();

	void go();

	Ogre::SceneManager* getSceneManager();
	Ogre::RenderWindow* getRenderWindow();
	SDL_Window* getSDLWindow();

	Ogre::Camera* getCamera();
	Ogre::Root* getRoot();

	void setFrameListener( Ogre::FrameListener* frameListener);

	void createCameraAndViewport(Ogre::ColourValue backgroundColor = Ogre::ColourValue::Blue);


};

