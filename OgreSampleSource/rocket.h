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
	Ogre::SceneNode* helperNode;
	Ogre::ParticleSystem* rocketTail;
public:
	Rocket(Ogre::String nodeName, double _scaleX = 100, double _scaleY = 100, double _scaleZ = 100, double _rocketMass = 10,
		btVector3 position = btVector3(0,0,0), btQuaternion rotation = btQuaternion(0,0,0,1))
	{
    	//set a scene node
		sceneNode = ICGAppFrame::getInstance()->getSceneMgr()->getRootSceneNode()->createChildSceneNode(nodeName+"Node");
		entity = ICGAppFrame::getInstance()->getSceneMgr()->createEntity(nodeName+"Entity",gRocketMeshName);
		sceneNode->attachObject(entity);
		helperNode = sceneNode->createChildSceneNode(nodeName+"HelperNode");
		//set a entity: the visual shape
		//entity = ICGAppFrame::getInstance()->getSceneMgr()->createEntity(nodeName+"Entity",Ogre::SceneManager::PT_CUBE);
		Ogre::AxisAlignedBox refbox = entity->getBoundingBox();
		Ogre::Vector3 aabbsize = refbox.getMaximum() - refbox.getMinimum();
		Ogre::Real xx = aabbsize.x;
		Ogre::Real yy = aabbsize.y;
		Ogre::Real zz = aabbsize.z;
		scaleX = _scaleX;
		scaleY = _scaleY;
		scaleZ = _scaleZ;
		rocketMass = _rocketMass;
		sceneNode->setScale(scaleX/xx, scaleY/yy, scaleZ/zz);
		//set scale
		helperNode->setScale(0.01,0.0001,0.0001); //scale : Full extent of box!
		helperNode->setOrientation(0,0,0,1);
		helperNode->setPosition(0,0,0);
		
		rocketTail = ICGAppFrame::getInstance()->getSceneMgr()->createParticleSystem("rocketTail", "Examples/JetEngine2");
		helperNode->attachObject(rocketTail);
		//set a rigidbody which is used for collision detection. 
		shape = new btBoxShape(btVector3(scaleX/2, scaleY/2, scaleZ/2));//box collision shape
		motionstate = new btDefaultMotionState(btTransform(rotation, position));//set motion

		btVector3 localInertia;
		shape->calculateLocalInertia(rocketMass, localInertia);

		btRigidBody::btRigidBodyConstructionInfo rigidCI(rocketMass, motionstate, shape, localInertia);
		rigidBody = new btRigidBody(rigidCI);
		rigidBody->setUserIndex(2);
		ICGAppFrame::getInstance()->addToDynamicsWorld(rigidBody);//register the rigidbody
	}
		
	~Rocket()
	{
		ICGAppFrame::getInstance()->getSceneMgr()->destroySceneNode(helperNode);
		
		//ICGAppFrame::getInstance()->getSceneMgr()->destroyParticleSystem(rocketTail);
	}
	
	void deleteTailEffect()
	{
		ICGAppFrame::getInstance()->getSceneMgr()->destroyParticleSystem(rocketTail);
	}

	btVector3 getLinearVelocity()
	{
		return rigidBody->getLinearVelocity();
	}
};
#endif