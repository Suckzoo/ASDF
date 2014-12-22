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
#define BEFORE_LAUNCH 1
#define LAUNCHING 2
#define FLYING 3
#define EXPLODE 4
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
	Ogre::Vector3 cPos;
	Ogre::Vector3 cDir;
	bool mKey_W;
	bool mKey_S;
	Ogre::Real accelFRMove;
	Ogre::Real vFRMove;
	bool mKey_A;
	bool mKey_D;
	Ogre::Real accelLRRotate;
	Ogre::Real vLRRotate;
	bool mKey_Q;
	bool mKey_E;
	Ogre::Real accelLRMove;
	Ogre::Real vLRMove;
	bool mKey_Z;
	bool mKey_C;
	Ogre::Real accelZRotate;
	Ogre::Real vZRotate;
	bool mKey_Space;
	Ogre::int32 phase;
	bool viewMode;

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
	void initCamera();
	void deleteFromDynamicsWorld(btRigidBody* _obj)
	{
		dynamicsWorld->removeRigidBody(_obj);
	}
	void trialFailed()
	{
		launchTrial++;
		mTrayMgr->setShot(launchTrial);
	}
	void collisionCheck();
	void processCamera();
	void processCameraZ();
	void trackCamera();
	void Shutdown();
	/** Upper-most method directly called from WinMain */
	bool go(void);
	void setPhase(int _phase)
	{
		phase = _phase;
		switch(phase)
		{
		case 1:
			viewMode = false;
			break;
		};
	}

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

//struct MyContactResultCallback : public btCollisionWorld::ContactResultCallback
//{
//	btScalar addSingleResult(btManifoldPoint& cp,
//		const btCollisionObjectWrapper* colObj0Wrap,
//		int partId0,
//		int index0,
//		const btCollisionObjectWrapper* colObj1Wrap,
//		int partId1,
//		int index1);
//};

#endif // #ifndef __ICGAppFrame_h_