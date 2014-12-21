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
	Sphere(Ogre::String nodeName, double _radius = 50,
		btVector3 position = btVector3(0,0,0), btQuaternion rotation = btQuaternion(0,0,0,1))
	{
		sceneNode = ICGAppFrame::getInstance()->getSceneMgr()->getRootSceneNode()->createChildSceneNode(nodeName+"SphereNode");
		entity = ICGAppFrame::getInstance()->getSceneMgr()->createEntity(nodeName+"SphereEntity", Ogre::SceneManager::PT_SPHERE);
		sceneNode->attachObject(entity);
		Ogre::Real refradius = entity->getBoundingRadius();
		sceneNode->setPosition(position.x(), position.y(), position.z());
		sceneNode->setOrientation(rotation.w(), rotation.x(), rotation.y(), rotation.z());
		sceneNode->setScale(_radius/refradius, _radius/refradius, _radius/refradius);
		radius = _radius;
		shape = new btSphereShape(radius);
		motionstate = new btDefaultMotionState(btTransform(rotation, position));
		btVector3 localInertia;
		shape->calculateLocalInertia(0, localInertia);
		btRigidBody::btRigidBodyConstructionInfo rigidCI(0, motionstate, shape, localInertia);
		rigidBody = new btRigidBody(rigidCI);
		rigidBody->setUserIndex(1);
		ICGAppFrame::getInstance()->addToDynamicsWorld(rigidBody);
	}
};
#endif