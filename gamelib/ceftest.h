#include "ogretests.h"


#include "cefprocess.h"
#include "guisheetmanagerbrowser.h"

#pragma once


class CefTest : public OgreTest, public GuiSheet {


	CefProcess* mCefProcess;


protected:
	void createScene() {

		Graphics::get().getSceneManager()->setAmbientLight(Ogre::ColourValue(1, 1, 1, 1));

		mCamera.setPosition(Ogre::Vector3(0, 0, 150));
		mCamera.lookAt(Ogre::Vector3(0, 0, 0.001));


		mCefProcess->getCefAppBrowser()->createBrowser("main", this, mInput );
			


		windowResized(Graphics::get().getRenderWindow());
	}

	void destroyScene() {
	
		mCefProcess->getCefAppBrowser()->destroyBrowser();
	}

	void onTest1Func(GuiArgList args) {
		mCefProcess->getCefAppBrowser()->getBrowser()->writeJavascript("func2", "", url);
	}

public:
	CefTest(Graphics* graphics, Input* input, CefProcess* cefProcess )
		: OgreTest(graphics, input),
		mCefProcess( cefProcess ),
		GuiSheet("statusbar.html")
	{


		GuiSheet::bind("test1", this, &CefTest::onTest1Func);
		
	}


	virtual bool frameStarted(const Ogre::FrameEvent& frameEvent) {
		if (!OgreTest::frameStarted(frameEvent)) return false;


		OIS::Mouse* mouse = mInput->getMouse();
		mouse->capture();
		
		OIS::Keyboard* keyboard = mInput->getKeyboard();
		keyboard->capture();

		return true;
	}


	//OIS EVENTS
	bool keyPressed(const OIS::KeyEvent& evt) {
	
		mCefProcess->getCefAppBrowser()->getBrowser()->keyPressed(evt.text, evt.key);
		return true;
	}
	bool keyReleased(const OIS::KeyEvent& evt) {

		mCefProcess->getCefAppBrowser()->getBrowser()->keyPressed(evt.text, evt.key);
		return true;
	}
	bool mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID btn) {
		mCefProcess->getCefAppBrowser()->getBrowser()->mousePressed(evt.state, btn);
		return true;
	}
	bool mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID btn) {
		mCefProcess->getCefAppBrowser()->getBrowser()->mouseReleased(evt.state, btn);
		return true;
	}
	bool mouseMoved(const OIS::MouseEvent& evt) {
		mCefProcess->getCefAppBrowser()->getBrowser()->mouseMoved(evt.state);

		return true;
	}

	void fiveTimesASecond() {

		CefDoMessageLoopWork();

	}
	void oneSecondElapsed() {


	}

	void windowResized( Ogre::RenderWindow* rw) {
		OgreTest::windowResized(rw);

		//resize the mouse dimensions
		unsigned int width, height, depth;
		int left, top;
		rw->getMetrics(width, height, depth, left, top);

		mCefProcess->getCefAppBrowser()->resize(width, height);
	}
};