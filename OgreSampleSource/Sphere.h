#ifndef __SPHERE__
#define __SPHERE__
#include <Ogre.h>
#include <OIS.h>
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include "Object.h"
#include "ICGAppFrame.h"
class Sphere : public Object
{
protected:
	Ogre::Real radius;
public:
	Sphere(ICGAppFrame* appFrame, Ogre::String nodeName)
	{
		sceneNode = appFrame->getSceneMgr()->getRootSceneNode()->createChildSceneNode(nodeName);
		entity = appFrame->mSceneMgr->createEntity("SphereEntity", Ogre::SceneManager::PT_SPHERE);
		radius = 100;
		btSphereShape* shape = new btSphereShape(radius);
		btDefaultMotionState* motionstate = btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), btVector(0,0,0));
		btVector3 localInertia;
		shape->calculateLocalInertia(0, localInertia);
		btRigidBody::btRigidBodyConstructionInfo rigidCI(0, motionstate, shape, localInertia);
		rigidBody = new btRigidBody(rigidCI);
		appFrame->getDynamicsWorld()->addRigidBody(rigidBody);
	}
};
#endif