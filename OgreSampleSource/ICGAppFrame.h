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

#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <exception>
using std::exception;

/**
 * ICGAppFrame 
 * Originated from MinimalOgre tutorial framework (http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Ogre+Wiki+Tutorial+Framework)
 * Multiple inheritance of C++ is not generally recommended, but it's not unusual to see that from professional code..
 */
class ICGAppFrame : public Ogre::FrameListener, public Ogre::WindowEventListener, public OIS::KeyListener, public OIS::MouseListener, public ICGOgreBites::SdkTrayListener
{
private:
	//Singleton style
	static ICGAppFrame* instance;
	ICGAppFrame(void);

protected:
	
	Ogre::Root *mTheRoot;
	Ogre::Camera* mCamera;
	Ogre::SceneManager* mSceneMgr;
	Ogre::RenderWindow* mWindow;
	Ogre::String mResourcesCfgFileName;
	Ogre::String mPluginsCfgFileName;

	ICGOgreBites::SdkTrayManager* mTrayMgr;
	
	bool mShutDown;
	bool mMouse_L;
	Ogre::Vector2 pos;
	bool mKey_W;
	bool mKey_S;
	bool mKey_A;
	bool mKey_D;
	bool mKey_Space;

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

	btDiscreteDynamicsWorld* dynamicsWorld;

	int launchTrial;
public:
	virtual ~ICGAppFrame(void);
	static ICGAppFrame* getInstance()
	{
		if(instance==NULL)
		{
			instance = new ICGAppFrame;
		}
		return instance;
	}
	Ogre::SceneManager* getSceneMgr() {return mSceneMgr;}
	void removeObjectFromDynamicsWorld(btRigidBody* _obj) {dynamicsWorld->removeRigidBody(_obj);}
	void addToDynamicsWorld(btRigidBody* _obj) {dynamicsWorld->addRigidBody(_obj);}
	void deleteFromDynamicsWorld(btRigidBody* _obj)
	{
		dynamicsWorld->removeRigidBody(_obj);
	}
	const int getNumTrial()
	{
		return launchTrial;
	}
	void trialFailed()
	{
		launchTrial++;
	}
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