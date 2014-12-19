#ifndef __CS482OBJ__
#define __CS482OBJ__


#include <Ogre.h>
#include <OIS.h>
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include "ICGSdkTrays.h"
#include "ICGSdkCameraMan.h"

#include "ICGDef.h"

#include "ICGUtils.h"
#include "ICGAppFrame.h"

class Object
{
protected:
	Ogre::SceneNode* sceneNode;
	Ogre::Entity* entity;
	btRigidBody* rigidBody;
	btCollisionShape* shape;
	btDefaultMotionState *motionstate;
public:
	Object() : sceneNode(nullptr), entity(nullptr), rigidBody(nullptr)
	{

	}
	virtual ~Object()
	{
		ICGAppFrame::getInstance()->removeObjectFromDynamicsWorld(rigidBody);
		delete rigidBody;
		delete motionstate;
		delete shape;
		
		ICGAppFrame::getInstance()->getSceneMgr()->destroySceneNode(sceneNode);
		ICGAppFrame::getInstance()->getSceneMgr()->destroyEntity(entity);
		
		delete sceneNode;
		delete entity;
		
		
	}
	Ogre::SceneNode* getSceneNode() {return this->sceneNode;}
	btRigidBody* getRigidBody() {return this->rigidBody;}
	void setPosition(Ogre::Real x, Ogre::Real y, Ogre::Real z)
	{
		btTransform trans;
		this->rigidBody->getMotionState()->getWorldTransform(trans);
		trans.setOrigin(btVector3(x, y, z));
		this->rigidBody->getMotionState()->setWorldTransform(trans);
		sceneNode->setPosition(x, y, z);
	}
	Ogre::Vector3 getPosition() 
	{
		return sceneNode->getPosition();
	}
	void setOrientation(Ogre::Real w, Ogre::Real x, Ogre::Real y, Ogre::Real z)
	{
		btTransform trans;
		this->rigidBody->getMotionState()->getWorldTransform(trans);
		trans.setRotation(btQuaternion(x, y, z, w));
		this->rigidBody->getMotionState()->setWorldTransform(trans);
		sceneNode->setOrientation(w, x, y, z);
	}
	Ogre::Quaternion getOrientation()
	{
		return sceneNode->getOrientation();
	}
	void setScale(Ogre::Real x, Ogre::Real y, Ogre::Real z)
	{
		sceneNode->setScale(x, y, z);
	}
	void applyMaterial(Ogre::String materialName)
	{
		Ogre::MaterialPtr mptr = Ogre::MaterialManager::getSingleton().getByName(materialName);
		entity->setMaterial(mptr);
	}
	void stepSimulation()
	{
		rigidBody->activate(true);
		btTransform trans;
		rigidBody->getMotionState()->getWorldTransform(trans);
		btVector3 pos = trans.getOrigin();
		btQuaternion rot = trans.getRotation();
		this->setPosition(pos.x(), pos.y(), pos.z());
		this->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
	}
	void applyCentralForce(double x, double y, double z)
	{
		this->rigidBody->applyCentralForce(btVector3(x, y, z));
	}
	/*void setCallback(){ //TODO
		
	}*/
};
#endif