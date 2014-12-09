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
	Rocket(Ogre::String nodeName, double _scaleX = 100, double _scaleY = 100, double _scaleZ = 100,double _rocketMass = 10)
	{
    	//set a scene node
		sceneNode = ICGAppFrame::getInstance()->getSceneMgr()->getRootSceneNode()->createChildSceneNode(nodeName);
		//set a entity: the visual shape
		entity = ICGAppFrame::getInstance()->getSceneMgr()->createEntity("RocketEntity", "ogrehead.mesh");
		sceneNode->attachObject(entity);
		
		//set scale
		sceneNode->setScale(_scaleX/100.0, _scaleY/100.0, _scaleZ/100.0);
		scaleX = _scaleX;
		scaleY = _scaleY;
		scaleZ = _scaleZ;
		rocketMass = _rocketMass;
		//set a rigidbody which is used for collision detection. 
		btCollisionShape* shape = new btBoxShape(btVector3(scaleX,scaleY,scaleZ));//box collision shape
		btDefaultMotionState *motionstate = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(0,0,0)));//set motion

		btVector3 localInertia;
		shape->calculateLocalInertia(0, localInertia);

		btRigidBody::btRigidBodyConstructionInfo rigidCI(rocketMass, motionstate, shape, localInertia);
		rigidBody = new btRigidBody(rigidCI);
		ICGAppFrame::getInstance()->addToDynamicsWorld(rigidBody);//register the rigidbody
	}
};
#endif