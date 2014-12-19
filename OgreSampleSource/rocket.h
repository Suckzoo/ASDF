/************************************************
filename: rocket
************************************************/

#ifndef __ROCKET__
#define __ROCKET__
#include <Ogre.h>
#include <OIS.h>
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>


class Rocket : public Object
{
protected:
	Ogre::Real scaleX, scaleY, scaleZ, rocketMass;
public:
	Rocket(Ogre::String nodeName, double _scaleX = 100, double _scaleY = 100, double _scaleZ = 100, double _rocketMass = 10,
		btVector3 position = btVector3(0,0,0), btQuaternion rotation = btQuaternion(0,0,0,1))
	{
    	//set a scene node
		sceneNode = ICGAppFrame::getInstance()->getSceneMgr()->getRootSceneNode()->createChildSceneNode(nodeName+"Node");
		//set a entity: the visual shape
		entity = ICGAppFrame::getInstance()->getSceneMgr()->createEntity(nodeName+"Entity",gRocketMeshName);
		sceneNode->attachObject(entity);
		Ogre::AxisAlignedBox refbox = entity->getBoundingBox();
		Ogre::Vector3 aabbsize = refbox.getMaximum() - refbox.getMinimum();
		Ogre::Real xx = aabbsize.x;
		Ogre::Real yy = aabbsize.y;
		Ogre::Real zz = aabbsize.z;
		//set scale

		sceneNode->setScale(_scaleX/xx, _scaleY/yy, _scaleZ/zz);
		sceneNode->setPosition(position.x(), position.y(), position.z());
		sceneNode->setOrientation(rotation.w(), rotation.x(), rotation.y(), rotation.z());
		scaleX = _scaleX;
		scaleY = _scaleY;
		scaleZ = _scaleZ;
		rocketMass = _rocketMass;
		//set a rigidbody which is used for collision detection. 
		shape = new btBoxShape(btVector3(scaleX, scaleY, scaleZ));//box collision shape
		motionstate = new btDefaultMotionState(btTransform(rotation, position));//set motion

		btVector3 localInertia;
		shape->calculateLocalInertia(rocketMass, localInertia);

		btRigidBody::btRigidBodyConstructionInfo rigidCI(rocketMass, motionstate, shape, localInertia);
		rigidBody = new btRigidBody(rigidCI);
		ICGAppFrame::getInstance()->addToDynamicsWorld(rigidBody);//register the rigidbody
	}

	btVector3 getLinearVelocity()
	{
		return rigidBody->getLinearVelocity();
	}
};
#endif