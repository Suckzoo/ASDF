//#includes
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
//#include "DrawableObject.h"
class SimulationObject
{
private:
	btVector3 pos;
	btQuaternion rot;
	btRigidBody *rigid;
	//DrawableObject* drawable;
public:
	btVector3 getPosition() {return pos;}
	btQuaternion getRotation() {return rot;}
	void stepSimulation()
	{
		btTransform trans;
		rigid->getMotionState()->getWorldTransform(trans);
		//drawable->setPosition(trans.getOrigin());
		//drawable->setRotation(trans.getRotation());
	}

};