/*
-----------------------------------------------------------------------------
ICGAppFrame.cpp

Sample scene application frame 
For [CS482] Interactive Computer Graphics practice class
Computer Graphics and Visualization Lab @ KAIST
-----------------------------------------------------------------------------
*/

#include "Object.h"
#include "Sphere.h"
#include "Box.h"
#include "rocket.h"
#include "ICGAppFrame.h"
#include "World.h"

HINSTANCE ghInstance = nullptr;
Ogre::String gCachedCmmandLine;

ICGAppFrame* ICGAppFrame::instance = nullptr;
ICGAppFrame::ICGAppFrame(void)
	: mTheRoot(nullptr)
	, mCamera(nullptr)
	, mSceneMgr(nullptr)
	, mWindow(nullptr)
	, mResourcesCfgFileName(Ogre::StringUtil::BLANK)
	, mPluginsCfgFileName(Ogre::StringUtil::BLANK)
	, mTrayMgr(nullptr)
	, mInputManager(nullptr)
	, mMouseInput(nullptr)
	, mKeyboardInput(nullptr)
	, mShutDown(false)
	, mMouse_L(false)
	, mKey_W(false)
	, mKey_S(false)
	, accelFRMove(1.0f)
	, vFRMove(0.0f)
	, mKey_A(false)
	, mKey_D(false)
	, accelLRRotate(1.0f)
	, vLRRotate(0.0f)
	, mKey_Q(false)
	, mKey_E(false)
	, accelLRMove(1.0f)
	, vLRMove(0.0f)
	, mKey_Z(false)
	, mKey_C(false)
	, accelZRotate(1.0f)
	, vZRotate(0.0f)
	, mKey_Space(false)
	, phase(BEFORE_LAUNCH)
	, viewMode(false)
	, mCameraNearClipDistance(1.0f)
	, dynamicsWorld(nullptr)
	, launchTrial(0)
{

}

ICGAppFrame::~ICGAppFrame(void)
{
	Cleanup();
}

/** Upper-most method directly called from WinMain */
bool ICGAppFrame::go(void)
{
	if(InitSystem() == false)
	{
		return false;
	}
	if(SetupScene() == false)
	{
		return false;
	}

	// At this point, mTheRoot should be ready.
	assert(mTheRoot);
	mTheRoot->startRendering();

	return true;
}

void ICGAppFrame::LoadStartupOptions()
{
	// Load up system configuration options from file, or parse command line options

	mSystemOptionsConfig.load(gSystemOptionsFileName);
	mCameraNearClipDistance = atof( mSystemOptionsConfig.getSetting("CameraNearClipDistance", "SystemConfig").c_str() );

	// To parse and use the command line options, refer to gCachedCmmandLine and Ogre::StringUtil

}

bool ICGAppFrame::InitSystem()
{
	LoadStartupOptions();

#ifdef _DEBUG
	mResourcesCfgFileName = "ICG_resources_d.cfg";
	mPluginsCfgFileName = "ICG_plugins_d.cfg";
#else
	mResourcesCfgFileName = "ICG_resources.cfg";
	mPluginsCfgFileName = "ICG_plugins.cfg";
#endif

	// construct Ogre::Root. this should be the first one to be created, and last one to be destroyed.
	mTheRoot = new Ogre::Root(mPluginsCfgFileName);

	//-------------------------------------------------------------------------------------
	// setup resources
	// Load resource paths from config file
	// After this block, when you call initialiseAllResourceGroups below,
	// all the resources specified in mResourcesCfgFileName will be loaded.
	Ogre::ConfigFile ResourceConfig;
	ResourceConfig.load(mResourcesCfgFileName);

	// Go through all sections & settings in the file
	Ogre::ConfigFile::SectionIterator SecItor = ResourceConfig.getSectionIterator();

	Ogre::String secName, typeName, archName;
	while (SecItor.hasMoreElements())
	{
		secName = SecItor.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = SecItor.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator SettingItor;
		for (SettingItor = settings->begin(); SettingItor != settings->end(); ++SettingItor)
		{
			typeName = SettingItor->first;
			archName = SettingItor->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				archName, typeName, secName);
		}
	}
	//-------------------------------------------------------------------------------------
	// configure
	// Show the configuration dialog and initialise the system
	// You can skip this and use root.restoreConfig() to load configuration
	// settings if you were sure there are valid ones saved in ogre.cfg
	if(mTheRoot->restoreConfig() || mTheRoot->showConfigDialog())
	{
		// If returned true, user clicked OK so initialise
		// Here we choose to let the system create a default rendering window by passing 'true'
		mWindow = mTheRoot->initialise(true, "[CS482] Sample Scene");
	}
	else
	{
		return false;
	}

	//-------------------------------------------------------------------------------------
	// choose scenemanager
	// Get the SceneManager, in this case a generic one
	// You may want to check other options, depends on the virtual world that you are trying to build
	mSceneMgr = mTheRoot->createSceneManager(Ogre::ST_GENERIC);

	//-------------------------------------------------------------------------------------
	// create camera
	// Create the camera
	mCamera = mSceneMgr->createCamera("PlayerCam");

	// Better be a configurable option.
	mCamera->setNearClipDistance(mCameraNearClipDistance);

	//-------------------------------------------------------------------------------------
	// create viewports
	// Create one viewport, entire window
	Ogre::Viewport* vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(Ogre::ColourValue(0,0,0));

	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio(
		Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
	//-------------------------------------------------------------------------------------
	// Set default mipmap level (NB some APIs ignore this)
	// Better check this, how affects entire texture management
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	//-------------------------------------------------------------------------------------
	// Create any resource listeners (for loading screens)
	//createResourceListener();

	//-------------------------------------------------------------------------------------
	// load resources
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	
	//create FrameListener
	Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	mWindow->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	mInputManager = OIS::InputManager::createInputSystem( pl );

	mKeyboardInput = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
	mMouseInput = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));

	mMouseInput->setEventCallback(this);
	mKeyboardInput->setEventCallback(this);

	//Set initial mouse clipping size
	windowResized(mWindow);

	//Register as a Window listener
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

	mTrayMgr = new ICGOgreBites::SdkTrayManager("InterfaceName", mWindow, mMouseInput, this);
	mTrayMgr->showFrameStats(ICGOgreBites::TL_BOTTOMLEFT);
	mTrayMgr->showLogo(ICGOgreBites::TL_BOTTOMRIGHT);
	mTrayMgr->setXCamera(mCamera->getRealPosition().x);
	mTrayMgr->setYCamera(mCamera->getRealPosition().y);
	mTrayMgr->setZCamera(mCamera->getRealPosition().z);

	mTheRoot->addFrameListener(this);

	//bullet engine loading
	btBroadphaseInterface* broadphase = new btDbvtBroadphase();
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	
	return true;
}

void ICGAppFrame::Cleanup()
{
	SAFE_DELETE(mTrayMgr)

	//Remove ourself as a Window listener
	Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
	windowClosed(mWindow);
	delete mTheRoot;
	mTheRoot = nullptr;
}

void ICGAppFrame::initCamera()
{
	mCamera->setPosition(Ogre::Vector3(0,0,0));
	mCamera->lookAt(Ogre::Vector3(0,0,500));
}
bool ICGAppFrame::SetupScene()
{
	// Position it at 500 in Z direction
	dynamicsWorld->setGravity(btVector3(0,0,0));
	initCamera();
	// This block will be the practice task for today
	{
		mSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox");
		//-------------------------------------------------------------------------------------
		// Create the scene
		Sphere* sphere = new Sphere("SphereNode1",50,btVector3(0,0,500));
		sphere->applyMaterial("Examples/BeachStones");
		sphere->setCastShadows(true);
		//sphere->setPosition(0,0,0);
		World::getInstance()->addObject(sphere);
		World::getInstance()->reloadRocket();
		//Rocket* rocket = new Rocket("RocketNode", 11,60,11,10,btVector3(0,0,0),btQuaternion((double)sqrt(2.0), 0, 0, (double)sqrt(2.0)));
		//World::getInstance()->registerRocket(rocket);
		//rocket->setPosition(-200,0,0);
		//World::getInstance()->addObject(rocket);

		//World::getInstance()->launchRocket(rocket);
		
		/*sphere->applyMaterial("Examples/Beachstones");
		Ogre::SceneNode* headNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("Sphere");
		Ogre::Entity* sphere = mSceneMgr->createEntity("SphereEntity", Ogre::SceneManager::PT_SPHERE); //PT_SPHERE radius = 100
		Ogre::MaterialPtr mptr = Ogre::MaterialManager::getSingleton().getByName("Examples/BeachStones");
		sphere->setMaterial(mptr);
		sphere->setMaterialName("Examples/Beachstones");*/

		
		/*headNode->attachObject(sphere);
		headNode->setPosition(0,0,0);
		headNode->setScale(1,1,1);
		*/
		// Set ambient light
		mSceneMgr->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1));

		Ogre::ParticleSystem* particleSystem = mSceneMgr->createParticleSystem("explosions", "Examples/Smoke");
		particleSystem->fastForward(10.0);
		sphere->getSceneNode()->attachObject(particleSystem);
		// Create a light
		/*Ogre::Light* MainLight = mSceneMgr->createLight("MainLight");
		MainLight->setPosition(20,80,50);
		MainLight->setDiffuseColour(Ogre::ColourValue(1,1,1));
		*/

		//Set shadow type
		mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

		//Set directional light
		Ogre::Light* directionalLight = mSceneMgr->createLight("directionalLight");
		directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
		directionalLight->setDiffuseColour(Ogre::ColourValue(.25, .25, .2));
		directionalLight->setSpecularColour(Ogre::ColourValue(.9, .9, .1));
		directionalLight->setDirection(Ogre::Vector3( 1, 0, 0 ));
		//-------------------------------------------------------------------------------------
	}

	return true;
}

/** You handle object's update routines here */
bool ICGAppFrame::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	// Use evt.timeSinceLastFrame for updating dynamic objects/events
	if(mWindow->isClosed())
	{
		return false;
	}
	if (mKey_Space) {
		if(!World::getInstance()->isRocketFired() && viewMode) {
			Ogre::Vector3 direction = mCamera->getDirection();
			btVector3 btDirection(direction.x, direction.y, direction.z);
			World::getInstance()->launchRocket(btDirection);
		}
	}

	if(mShutDown)
	{
		return false;
	}
	dynamicsWorld->stepSimulation(1/60.f, 10);
	World::getInstance()->stepSimulation();
	//Need to capture/update each device
	mKeyboardInput->capture();
	mMouseInput->capture();

	mTrayMgr->frameRenderingQueued(evt);

	// Update stuff when the dialog isn't up
	if (!mTrayMgr->isDialogVisible())
	{

	}

	switch(phase) {
	case BEFORE_LAUNCH:
		if(!viewMode) {
			processCamera();
		}
		break;
	case FLYING:
		trackCamera();
		break;
	default:
		break;
	}

	mTrayMgr->setXCamera(mCamera->getRealPosition().x);
	mTrayMgr->setYCamera(mCamera->getRealPosition().y);
	mTrayMgr->setZCamera(mCamera->getRealPosition().z);
	
	//Sleep(1000.0/60.0);
	return true;
}

bool ICGAppFrame::keyPressed( const OIS::KeyEvent &arg )
{
	if (mTrayMgr->isDialogVisible()) 
	{
		return true;   // don't process any more keys if dialog is up
	}

	if (arg.key == OIS::KC_Z)   // toggle visibility of advanced frame stats
	{
		//mTrayMgr->toggleAdvancedFrameStats();
	}

	if (arg.key == OIS::KC_R)   // cycle polygon rendering mode
	{
		Ogre::String newVal;
		Ogre::PolygonMode pm;

		switch (mCamera->getPolygonMode())
		{
		case Ogre::PM_SOLID:
			newVal = "Wireframe";
			pm = Ogre::PM_WIREFRAME;
			break;
		case Ogre::PM_WIREFRAME:
			newVal = "Points";
			pm = Ogre::PM_POINTS;
			break;
		default:
			newVal = "Solid";
			pm = Ogre::PM_SOLID;
		}

		mCamera->setPolygonMode(pm);
	}
	else if(arg.key == OIS::KC_F5)   // refresh all textures
	{
		Ogre::TextureManager::getSingleton().reloadAll();
	}
	else if (arg.key == OIS::KC_SYSRQ)   // take a screenshot
	{
		mWindow->writeContentsToTimestampedFile("screenshot", ".jpg");
	}
	else if (arg.key == OIS::KC_ESCAPE)
	{
		mShutDown = true;
	}
	
	if(arg.key == OIS::KC_W) {
		mKey_W = true;
	}
	if(arg.key == OIS::KC_S) {
		mKey_S = true;
	}
	if(arg.key == OIS::KC_A) {
		mKey_A = true;
	}
	if(arg.key == OIS::KC_D) {
		mKey_D = true;
	}
	if(arg.key == OIS::KC_Q) {
		mKey_Q = true;
	}
	if(arg.key == OIS::KC_E) {
		mKey_E = true;
	}
	if(arg.key == OIS::KC_Z) {
		mKey_Z = true;
	}
	if(arg.key == OIS::KC_C) {
		mKey_C = true;
	}
	if(arg.key == OIS::KC_TAB) {
		switch(phase) {
		case BEFORE_LAUNCH:
			if(!viewMode) {
				vFRMove = 0.0f;
				vLRRotate = 0.0f;
				vLRMove = 0.0f;
				vZRotate = 0.0f;
				cPos = mCamera->getRealPosition();
				cDir = mCamera->getRealDirection();
				mCamera->setPosition(Ogre::Vector3(0,0,0));
				mCamera->lookAt(Ogre::Vector3(0,0,500));
			}
			else {
				mCamera->setPosition(cPos);
				mCamera->setDirection(cDir);
			}
			break;
		default:
			break;
		}
		viewMode = !viewMode;
	}
	if(arg.key == OIS::KC_I) {
		vFRMove = 0.0f;
		vLRRotate = 0.0f;
		vLRMove = 0.0f;
		vZRotate = 0.0f;
		initCamera();
	}
	if(arg.key == OIS::KC_SPACE) {
		mKey_Space = true;
	}
	return true;
}

bool ICGAppFrame::keyReleased( const OIS::KeyEvent &arg )
{
	if(arg.key == OIS::KC_W) {
		mKey_W = false;
	}
	if(arg.key == OIS::KC_S) {
		mKey_S = false;
	}
	if(arg.key == OIS::KC_A) {
		mKey_A = false;
	}
	if(arg.key == OIS::KC_D) {
		mKey_D = false;
	}
	if(arg.key == OIS::KC_Q) {
		mKey_Q = false;
	}
	if(arg.key == OIS::KC_E) {
		mKey_E = false;
	}
	if(arg.key == OIS::KC_Z) {
		mKey_Z = false;
	}
	if(arg.key == OIS::KC_C) {
		mKey_C = false;
	}
	if(arg.key == OIS::KC_SPACE) {
		mKey_Space = false;
		phase = FLYING;
	}
	return true;
}

bool ICGAppFrame::mouseMoved( const OIS::MouseEvent &arg )
{
	if (mTrayMgr->injectMouseMove(arg)) 
	{
		return true;
	}
	if(mMouse_L) {
		Ogre::Vector2 newPos = Ogre::Vector2(arg.state.X.abs, arg.state.Y.abs);
		mCamera->rotate(mCamera->getRealUp(), Ogre::Radian(Ogre::Degree(-0.1)*(newPos.x-pos.x)));
		mCamera->rotate(mCamera->getRealDirection().crossProduct(mCamera->getRealUp()), Ogre::Radian(Ogre::Degree(-0.1)*(newPos.y-pos.y)));
		pos = newPos;
		switch(phase) {
		case BEFORE_LAUNCH:
			if(viewMode) {
				/*vFRMove = 0.0f;
				vLRRotate = 0.0f;
				vLRMove = 0.0f;
				vZRotate = 0.0f;
				cPos = mCamera->getRealPosition();
				cDir = mCamera->getRealDirection();
				mCamera->setPosition(Ogre::Vector3(0,0,0));
				mCamera->lookAt(Ogre::Vector3(0,0,500));*/
				Ogre::Vector3 cameraDirection = mCamera->getDirection();
				cameraDirection /= cameraDirection.length();
				Ogre::Quaternion q;
				Ogre::Vector3 refVector(0,1,0);
				Ogre::Real d = refVector.dotProduct(cameraDirection);
				if(d > 0.999999)
				{
					q.w = 1;
					q.x = 0;
					q.y = 0;
					q.z = 0;
				}
				else if(d < -0.999999)
				{
					q.w = 0;
					q.x = 1;
					q.y = 0;
					q.z = 0;
				}
				else
				{
					Ogre::Real s = sqrt( (1+d)*2 );
					Ogre::Real invs = 1 / s;

					Ogre::Vector3 c = refVector.crossProduct(cameraDirection);
					q.x = c.x * invs;
					q.y = c.y * invs;
					q.z = c.z * invs;
					q.w = s * 0.5f;
					q.normalise();
				}
				World::getInstance()->setRocketOrientation(q.w, q.x, q.y, q.z);
			}
			else {
			}
			break;
		default:
			break;
		}
	}
	return true;
}

bool ICGAppFrame::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if (mTrayMgr->injectMouseDown(arg, id)) 
	{
		return true;
	}
	if(id == OIS::MouseButtonID::MB_Left) {
		mMouse_L = true;
		pos = Ogre::Vector2(arg.state.X.abs, arg.state.Y.abs);
	}
	return true;
}

bool ICGAppFrame::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if (mTrayMgr->injectMouseUp(arg, id)) 
	{
		return true;
	}
	if(id == OIS::MouseButtonID::MB_Left) {
		mMouse_L = false;
	}
	return true;
}

void ICGAppFrame::windowResized(Ogre::RenderWindow* rw)
{
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState &ms = mMouseInput->getMouseState();
	ms.width = width;
	ms.height = height;
}

void ICGAppFrame::windowClosed(Ogre::RenderWindow* rw)
{
	//Only close for window that created OIS (the main window in these demos)
	if( rw == mWindow )
	{
		if( mInputManager )
		{
			mInputManager->destroyInputObject( mMouseInput );
			mInputManager->destroyInputObject( mKeyboardInput );

			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager = nullptr;
		}
	}	
}

void ICGAppFrame::processCamera()
{
	Ogre::Real m_accelFRMove = 0.001*accelFRMove;
	if (mKey_W && vFRMove < accelFRMove) vFRMove += 10*m_accelFRMove;
	if (mKey_S && vFRMove > -accelFRMove) vFRMove -= 10*m_accelFRMove;
	if (!(mKey_W ^ mKey_S)) {
		if(vFRMove > 0) {
			if(vFRMove < m_accelFRMove) vFRMove = 0.0f;
			else vFRMove -= m_accelFRMove;
		}
		else if(vFRMove < 0) {
			if(vFRMove > -m_accelFRMove) vFRMove = 0.0f;
			else vFRMove += m_accelFRMove;
		}
	}
	if(vFRMove) mCamera->setPosition(mCamera->getPosition() + mCamera->getRealDirection()*0.2*vFRMove);

	Ogre::Real m_accelLRRotate = 0.001*accelLRRotate;
	if (mKey_A && vLRRotate < accelLRRotate) vLRRotate += 10*m_accelLRRotate;
	if (mKey_D && vLRRotate > -accelLRRotate) vLRRotate -= 10*m_accelLRRotate;
	if (!(mKey_A ^ mKey_D)) {
		if(vLRRotate > 0) {
			if(vLRRotate < m_accelLRRotate) vLRRotate = 0.0f;
			else vLRRotate -= m_accelLRRotate;
		}
		else if(vLRRotate < 0) {
			if(vLRRotate > -m_accelLRRotate) vLRRotate = 0.0f;
			else vLRRotate += m_accelLRRotate;
		}
	}
	if(vLRRotate) mCamera->rotate(mCamera->getRealUp(), Ogre::Radian(Ogre::Degree(0.05*vLRRotate)));

	Ogre::Real m_accelLRMove = 0.001*accelLRMove;
	if (mKey_Q && vLRMove < accelLRMove) vLRMove += 10*m_accelLRMove;
	if (mKey_E && vLRMove > -accelLRMove) vLRMove -= 10*m_accelLRMove;
	if (!(mKey_Q ^ mKey_E)) {
		if(vLRMove > 0) {
			if(vLRMove < m_accelLRMove) vLRMove = 0.0f;
			else vLRMove -= m_accelLRMove;
		}
		else if(vLRMove < 0) {
			if(vLRMove > -m_accelLRMove) vLRMove = 0.0f;
			else vLRMove += m_accelLRMove;
		}
	}
	if(vLRMove) mCamera->setPosition(mCamera->getPosition() + mCamera->getRealDirection().crossProduct(mCamera->getRealUp())*-0.2*vLRMove);

	Ogre::Real m_accelZRotate = 0.001*accelZRotate;
	if (mKey_Z && vZRotate < accelZRotate) vZRotate += 10*m_accelZRotate;
	if (mKey_C && vZRotate > -accelZRotate) vZRotate -= 10*m_accelZRotate;
	if (!(mKey_Z ^ mKey_C)) {
		if(vZRotate > 0) {
			if(vZRotate < m_accelZRotate) vZRotate = 0.0f;
			else vZRotate -= m_accelZRotate;
		}
		else if(vZRotate < 0) {
			if(vZRotate > -m_accelZRotate) vZRotate = 0.0f;
			else vZRotate += m_accelZRotate;
		}
	}
	if(vZRotate) mCamera->rotate(mCamera->getRealDirection(), Ogre::Radian(Ogre::Degree(0.1*vZRotate)));
}

void ICGAppFrame::trackCamera()
{
	Ogre::Vector3 offset = Ogre::Vector3(0.0f, 0.0f, -200.0f);
	btVector3 rocketDir = World::getInstance()->getRocket()->getLinearVelocity();
	mCamera->setPosition(World::getInstance()->getRocketPosition() + offset);
	mCamera->setDirection(Ogre::Vector3(rocketDir.getX(), rocketDir.getY(), rocketDir.getZ()));
}

void ICGAppFrame::collisionCheck()
{
	int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
	for (int i=0;i<numManifolds;i++)
	{
		btPersistentManifold* contactManifold =  dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* obA = (btCollisionObject*)contactManifold->getBody0();
		btCollisionObject* obB = (btCollisionObject*)contactManifold->getBody1();

		int numContacts = contactManifold->getNumContacts();
		for (int j=0;j<numContacts;j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			if (pt.getDistance()<=0.f)
			{
				if(obA->getUserIndex()==1)
				{
					if(obB->getUserIndex()==2)
					{
						World::getInstance()->contactedWithPlanet();
					}
				}
				else if(obA->getUserIndex()==2)
				{
					if(obB->getUserIndex()==1)
					{
						World::getInstance()->contactedWithPlanet();
					}
					else if(obB->getUserIndex()==3)
					{
						World::getInstance()->contactedWithTarget();
					}
				}
				else if(obA->getUserIndex()==3)
				{
					if(obB->getUserIndex()==2)
					{
						World::getInstance()->contactedWithTarget();
					}
				}
				//const btVector3& ptA = pt.getPositionWorldOnA();
				//const btVector3& ptB = pt.getPositionWorldOnB();
				//const btVector3& normalOnB = pt.m_normalWorldOnB;
			}
		}
	}
}

//void ICGAppFrame::collisionCheck(btRigidBody* rig1, btRigidBody* rig2)
//{
//	MyContactResultCallback callback;
//	dynamicsWorld->contactPairTest(rig1, rig2, callback);
//}
//
//btScalar MyContactResultCallback::addSingleResult(btManifoldPoint& cp,
//		const btCollisionObjectWrapper* colObj0Wrap,
//		int partId0,
//		int index0,
//		const btCollisionObjectWrapper* colObj1Wrap,
//		int partId1,
//		int index1)
//{
//	//colObj0Wrap->getCollisionObject()->getUserIndex();
//	//User index guide :
//	//1 is planet object.
//	//2 is rocket launched.
//	//3 is target.
//	if(colObj0Wrap->getCollisionObject()->getUserIndex()==1) {
//		if(colObj1Wrap->getCollisionObject()->getUserIndex()==2) {
//			World::getInstance()->contactedWithPlanet();
//		}
//		//else if(colObj1Wrap->getCollisionObject()->getUserIndex()==2){
//		//}
//	} else if(colObj0Wrap->getCollisionObject()->getUserIndex()==2) {
//		if(colObj1Wrap->getCollisionObject()->getUserIndex()==1) {
//			World::getInstance()->contactedWithPlanet();
//		}
//		else if(colObj1Wrap->getCollisionObject()->getUserIndex()==3){
//			World::getInstance()->contactedWithTarget();
//		}
//	} else if(colObj0Wrap->getCollisionObject()->getUserIndex()==3) {
//		if(colObj1Wrap->getCollisionObject()->getUserIndex()==1) {
//			World::getInstance()->contactedWithTarget();
//		}
//		//else if(colObj1Wrap->getCollisionObject()->getUserIndex()==1){
//		//}
//	}
//	return 1.0f;
//}

// The place where things begin
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

#ifdef __cplusplus
extern "C" {
#endif
	INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
	{
		// Cache the hInst and strCmdLine for later use if needed
		ghInstance = hInst;
		gCachedCmmandLine = strCmdLine;

		// Create application object
		//ICGAppFrame app;

		try 
		{
			ICGAppFrame::getInstance()->go();
		} 
		catch( Ogre::Exception& e ) 
		{
			MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		}

		return 0;
	}

#ifdef __cplusplus
}
#endif