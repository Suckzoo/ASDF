#ifndef __BOX__
#define __BOX__
#include <Ogre.h>
#include <OIS.h>
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include "Object.h"
class Box : public Object
{
protected:
	Ogre::Vector3 boxHalfExtents;
public:
	Box(Ogre::String nodeName, double halfExtentX = 100, double halfExtentY = 100, double halfExtentZ = 100)
	{
		sceneNode = ICGAppFrame::getInstance()->getSceneMgr()->getRootSceneNode()->createChildSceneNode(nodeName);
		entity = ICGAppFrame::getInstance()->getSceneMgr()->createEntity("SphereEntity", Ogre::SceneManager::PT_CUBE);
		sceneNode->attachObject(entity);
		Ogre::Vector3 halfExtents = Ogre::Vector3(halfExtentX, halfExtentY, halfExtentZ);
		btBoxShape* shape = new btBoxShape(halfExtents);
		btDefaultMotionState *motionstate = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(0,0,0)));
		btVector3 localInertia;
		shape->calculateLocalInertia(0, localInertia);
		btRigidBody::btRigidBodyConstructionInfo rigidCI(0, motionstate, shape, localInertia);
		rigidBody = new btRigidBody(rigidCI);
		ICGAppFrame::getInstance()->addToDynamicsWorld(rigidBody);
	}
};
#endif