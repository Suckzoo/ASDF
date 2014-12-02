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

class Object
{
protected:
	Ogre::SceneNode* sceneNode;
	Ogre::Entity* entity;
	btRigidBody* rigidBody;
public:
	Object() : sceneNode(nullptr), entity(nullptr), rigidBody(nullptr)
	{

	}
	void setPosition(Ogre::Real x, Ogre::Real y, Ogre::Real z)
	{
		sceneNode->setPosition(x, y, z);
	}
	Ogre::Vector3 getPosition() 
	{
		return sceneNode->getPosition();
	}
	void setOrientation(Ogre::Real w, Ogre::Real x, Ogre::Real y, Ogre::Real z)
	{
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
		btTransform trans;
		rigidBody->getMotionState()->getWorldTransform(trans);
		btVector3 pos = trans.getOrigin();
		btQuaternion rot = trans.getRotation();
		this->setPosition(pos.x(), pos.y(), pos.z());
		this->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
	}
	void registerDynamicsWorld(btDiscreteDynamicsWorld* world)
	{
		world->addRigidBody(this->rigidBody);
	}
};
#endif