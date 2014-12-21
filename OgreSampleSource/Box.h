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
	Box(Ogre::String nodeName, double halfExtentX = 100, double halfExtentY = 100, double halfExtentZ = 100,
		btVector3 position = btVector3(0,0,0), btQuaternion rotation = btQuaternion(0,0,0,1))
	{
		sceneNode = ICGAppFrame::getInstance()->getSceneMgr()->getRootSceneNode()->createChildSceneNode(nodeName+"BoxNode");
		entity = ICGAppFrame::getInstance()->getSceneMgr()->createEntity(nodeName+"BoxEntity", Ogre::SceneManager::PT_CUBE);
		sceneNode->attachObject(entity);
		sceneNode->setPosition(position.x(), position.y(), position.z());
		btVector3 halfExtents = btVector3(halfExtentX, halfExtentY, halfExtentZ);
		btBoxShape* shape = new btBoxShape(halfExtents);
		btDefaultMotionState *motionstate = new btDefaultMotionState(btTransform(rotation, position));
		btVector3 localInertia;
		shape->calculateLocalInertia(0, localInertia);
		btRigidBody::btRigidBodyConstructionInfo rigidCI(0, motionstate, shape, localInertia);
		rigidBody = new btRigidBody(rigidCI);
		rigidBody->setUserIndex(3);
		ICGAppFrame::getInstance()->addToDynamicsWorld(rigidBody);
	}
};
#endif