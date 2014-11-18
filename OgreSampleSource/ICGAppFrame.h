/*
-----------------------------------------------------------------------------
ICGAppFrame.h

Sample scene application frame 
For [CS482] Interactive Computer Graphics practice class
Computer Graphics and Visualization Lab @ KAIST
-----------------------------------------------------------------------------
*/

#ifndef __ICGAppFrame_h_
#define __ICGAppFrame_h_

#pragma warning (disable : 4251) // There are too many

#include "Ogre.h"
#include "OIS.h"
#include "ICGSdkTrays.h"
#include "ICGSdkCameraMan.h"

#include "ICGDef.h"

#include "ICGUtils.h"

/**
 * ICGAppFrame 
 * Originated from MinimalOgre tutorial framework (http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Ogre+Wiki+Tutorial+Framework)
 * Multiple inheritance of C++ is not generally recommended, but it's not unusual to see that from professional code..
 */
class ICGAppFrame : public Ogre::FrameListener, public Ogre::WindowEventListener, public OIS::KeyListener, public OIS::MouseListener, public ICGOgreBites::SdkTrayListener
{
protected:
	Ogre::Root *mTheRoot;
	Ogre::Camera* mCamera;
	Ogre::SceneManager* mSceneMgr;
	Ogre::RenderWindow* mWindow;
	Ogre::String mResourcesCfgFileName;
	Ogre::String mPluginsCfgFileName;

	ICGOgreBites::SdkTrayManager* mTrayMgr;
	
	bool mShutDown;

	// OIS Input devices
	OIS::InputManager* mInputManager;
	OIS::Mouse* mMouseInput;
	OIS::Keyboard* mKeyboardInput;

	Ogre::ConfigFile mSystemOptionsConfig;

	/** For initial demonstration of config file parsing..
	 * You need to set this NearClipDistance very carefully, especially for large world.
	 * This affects the precision of the depth buffer.
	 * You call setNearClipDistance to camera to actually set this */
	Ogre::Real mCameraNearClipDistance;

public:
	ICGAppFrame(void);
	virtual ~ICGAppFrame(void);
	
	/** Upper-most method directly called from WinMain */
	bool go(void);

protected:

	virtual void LoadStartupOptions();
	
	virtual bool InitSystem();
	virtual void Cleanup();

	/** Root and SceneManager are ready before comes here */
	virtual bool SetupScene();

	// Ogre::FrameListener
	/** You handle object's update routines here */
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

	//---------------------------------------- 
	// Callbacks for various events

	// OIS::KeyListener
	virtual bool keyPressed( const OIS::KeyEvent &arg );
	virtual bool keyReleased( const OIS::KeyEvent &arg );

	// OIS::MouseListener
	virtual bool mouseMoved( const OIS::MouseEvent &arg );
	virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

	// Ogre::WindowEventListener
	virtual void windowResized(Ogre::RenderWindow* rw);
	virtual void windowClosed(Ogre::RenderWindow* rw);

	//---------------------------------------- 
};

#endif // #ifndef __ICGAppFrame_h_