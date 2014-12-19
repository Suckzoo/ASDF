#ifndef __SPHERE__
#define __SPHERE__
#include <Ogre.h>
#include <OIS.h>
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include "Object.h"
class Sphere : public Object
{
protected:
	Ogre::Real radius;
public:
	Sphere(Ogre::String nodeName, double _radius = 100)
	{
		sceneNode = ICGAppFrame::getInstance()->getSceneMgr()->getRootSceneNode()->createChildSceneNode(nodeName+"Node");
		entity = ICGAppFrame::getInstance()->getSceneMgr()->createEntity(nodeName+"Entity", Ogre::SceneManager::PT_SPHERE);
		sceneNode->attachObject(entity);
		sceneNode->setScale(_radius/100.0, _radius/100.0, _radius/100.0);
		radius = _radius;
		shape = new btSphereShape(radius);
		motionstate = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(0,0,0)));
		btVector3 localInertia;
		shape->calculateLocalInertia(0, localInertia);
		btRigidBody::btRigidBodyConstructionInfo rigidCI(0, motionstate, shape, localInertia);
		rigidBody = new btRigidBody(rigidCI);
		ICGAppFrame::getInstance()->addToDynamicsWorld(rigidBody);
	}
};
#endif