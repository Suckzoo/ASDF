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
public:
	Rocket(Ogre::String nodeName, double _scaleX = 100, double _scaleY = 100, double _scaleZ = 100, double _rocketMass = 10,
		btVector3 position = btVector3(0,0,0), btQuaternion rotation = btQuaternion(0,0,0,1))
	{
    	//set a scene node
		sceneNode = ICGAppFrame::getInstance()->getSceneMgr()->getRootSceneNode()->createChildSceneNode(nodeName+"Node");
		helperNode = sceneNode->createChildSceneNode(nodeName+"HelperNode");
		//set a entity: the visual shape
		entity = ICGAppFrame::getInstance()->getSceneMgr()->createEntity(nodeName+"Entity",gRocketMeshName);
		//entity = ICGAppFrame::getInstance()->getSceneMgr()->createEntity(nodeName+"Entity",Ogre::SceneManager::PT_CUBE);
		helperNode->attachObject(entity);
		Ogre::AxisAlignedBox refbox = entity->getBoundingBox();
		Ogre::Vector3 aabbsize = refbox.getMaximum() - refbox.getMinimum();
		Ogre::Real xx = aabbsize.x;
		Ogre::Real yy = aabbsize.y;
		Ogre::Real zz = aabbsize.z;
		//set scale
		sceneNode->setScale(_scaleX/xx, _scaleY/yy, _scaleZ/zz); //scale : Full extent of box!
		sceneNode->setOrientation(rotation.w(), rotation.x(), rotation.y(), rotation.z());
		sceneNode->setPosition(position.x(), position.y(), position.z());
		
		scaleX = _scaleX;
		scaleY = _scaleY;
		scaleZ = _scaleZ;
		rocketMass = _rocketMass;
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
	}

	btVector3 getLinearVelocity()
	{
		return rigidBody->getLinearVelocity();
	}
};
#endif