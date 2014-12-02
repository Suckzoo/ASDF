/*
-----------------------------------------------------------------------------
ICGAppFrame.cpp

Sample scene application frame 
For [CS482] Interactive Computer Graphics practice class
Computer Graphics and Visualization Lab @ KAIST
-----------------------------------------------------------------------------
*/

#include "ICGAppFrame.h"

HINSTANCE ghInstance = nullptr;
Ogre::String gCachedCmmandLine;

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
	, mKey_W(false)
	, mKey_S(false)
	, mKey_A(false)
	, mKey_D(false)
	, mCameraNearClipDistance(1.0f)
	, dynamicsWorld(nullptr)
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

bool ICGAppFrame::SetupScene()
{
	// Position it at 500 in Z direction
	mCamera->setPosition(Ogre::Vector3(0,0,500));
	// Look back along -Z
	mCamera->lookAt(Ogre::Vector3(0,0,0));
	// This block will be the practice task for today
	{
		mSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox");
		//-------------------------------------------------------------------------------------
		// Create the scene
		
		Ogre::SceneNode* headNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("Sphere");
		Ogre::Entity* sphere = mSceneMgr->createEntity("SphereEntity", Ogre::SceneManager::PT_SPHERE); //PT_SPHERE radius = 100
		Ogre::MaterialPtr mptr = Ogre::MaterialManager::getSingleton().getByName("Examples/BeachStones");
		sphere->setMaterial(mptr);
		//sphere->setMaterialName("Examples/Beachstones");
		
		headNode->attachObject(sphere);
		headNode->setPosition(0,0,0);
		headNode->setScale(1,1,1);
		// Set ambient light
		mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));

		// Create a light
		//Ogre::Light* MainLight = mSceneMgr->createLight("MainLight");
		//MainLight->setPosition(20,80,50);
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

	if(mShutDown)
	{
		return false;
	}

	//Need to capture/update each device
	mKeyboardInput->capture();
	mMouseInput->capture();

	mTrayMgr->frameRenderingQueued(evt);

	// Update stuff when the dialog isn't up
	if (!mTrayMgr->isDialogVisible())
	{

	}
	
	if (mKey_W) {
		mCamera->setPosition(mCamera->getPosition() + mCamera->getRealDirection()*0.2);
	}
	if (mKey_S) {
		mCamera->setPosition(mCamera->getPosition() + mCamera->getRealDirection()*-0.2);
	}
	if (mKey_A) {
		mCamera->rotate(Ogre::Vector3(0, 1, 0), Ogre::Radian(Ogre::Degree(0.2)));
	}
	if (mKey_D) {
		mCamera->rotate(Ogre::Vector3(0, 1, 0), Ogre::Radian(Ogre::Degree(-0.2)));
	}

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
		mTrayMgr->toggleAdvancedFrameStats();
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
	return true;
}

bool ICGAppFrame::mouseMoved( const OIS::MouseEvent &arg )
{
	if (mTrayMgr->injectMouseMove(arg)) 
	{
		return true;
	}
	return true;
}

bool ICGAppFrame::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if (mTrayMgr->injectMouseDown(arg, id)) 
	{
		return true;
	}
	return true;
}

bool ICGAppFrame::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if (mTrayMgr->injectMouseUp(arg, id)) 
	{
		return true;
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
		ICGAppFrame app;

		try 
		{
			app.go();
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