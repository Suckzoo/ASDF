#ifndef __SPHERE__
#define __SPHERE__
#include "Object.h"
#include "ICGAppFrame.h"
class Sphere : public Object
{
protected:
	Ogre::Real radiusX, radiusY, radiusZ;
public:
	Sphere(Ogre::String nodeName, ICGAppFrame* appFrame)
	{
		sceneNode = appFrame->mSceneMgr->getRootSceneNode()->createChildSceneNode(nodeName);
		entity = appFrame->mSceneMgr->createEntity("SphereEntity", Ogre::SceneManager::PT_SPHERE);
		radiusX = radiusY = radiusZ = 100;
		rigidBody = 
	}
};
#endif