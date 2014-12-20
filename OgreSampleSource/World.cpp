#include "World.h"
#include <memory>
#include <typeinfo>
using std::shared_ptr;

//FILE *simulLog = fopen("output.log","wt");

World::World()
	: m_pWorld(list<shared_ptr<Object> >())
	, isRocketLaunched(false)
{

}

World* World::instance = nullptr;

World* World::getInstance()
{
	if(instance == nullptr)
	{
		instance = new World();
	}
	return instance;
}

void World::addObject(Object* obj)
{
	m_pWorld.push_back(shared_ptr<Object>(obj));
}

void World::stepSimulation()
{
	//m_pLaunchedRocket.get()->stepSimulation();
	for(auto it = m_pWorld.begin(); it != m_pWorld.end(); it++)
	{		
		if(!(*it)) {
			m_pWorld.erase(it);
			continue;
		}
		(*it)->stepSimulation();
	}
	//TODO : Collision detection for m_pLaunchedRocket and m_pWorld[i].
	if(isRocketLaunched) {
		//fprintf(simulLog, "Rocket launched.\n");
		Ogre::Vector3 l = (*m_pLaunchedRocket).get()->getPosition();
		//fprintf(simulLog, "Current rocket position : (%lf, %lf, %lf)\n", l.x, l.y, l.z);
		btVector3 netForce(0,0,0);
		for(auto it = m_pWorld.begin(); it != m_pWorld.end(); it++)
		{
			if(it == m_pLaunchedRocket)
			{
				continue;
			}
			else
			{
				Ogre::Vector3 locPlanet = (*it).get()->getPosition();
				Ogre::Vector3 locRocket = (*m_pLaunchedRocket).get()->getPosition();
				Ogre::Vector3 dist = locPlanet - locRocket;
				if(dist.length()<=1000.0f) { //force area. can be fixed. (minor priority)
					//Pseudocode section!
					//Collision detection -> Chih-yu, could you implement the method of it?
					//if(isCollided) {  -> you have to implement "isCollided" method.
					//	destroyRocket();
					//}
					//else {
						double r = dist.length();
						dist = dist / dist.length();
						btVector3 forceApplied = (1000000.0/(r*r)) * btVector3(dist.x, dist.y, dist.z);
						netForce += forceApplied;
						//fprintf(simulLog, "Force applied by planet : (%lf, %lf, %lf)\n", forceApplied.x(), forceApplied.x(), forceApplied.x());
					//}
				}
			}
		}
		//fprintf(simulLog, "Total force applied is : (%lf, %lf, %lf)\n", netForce.x(), netForce.y(), netForce.z());
		btVector3 v = ((Rocket*)((*m_pLaunchedRocket).get()))->getLinearVelocity();
		//fprintf(simulLog, "Current velocity of rocket is : (%lf, %lf, %lf)\n", v.x(), v.y(), v.z());
		((Rocket*)(*m_pLaunchedRocket).get())
			->applyCentralForce(netForce.x(), netForce.y(), netForce.z());
		Ogre::Quaternion q;
		Ogre::Vector3 refVector(0,1,0);
		Ogre::Vector3 velocity(v.x(), v.y(), v.z());
		if(velocity.length()>0.0001) {
			if(refVector.dotProduct(velocity)/velocity.length()>0.999999) {
				q.w = 1;
				q.x = 0;
				q.y = 0;
				q.z = 0;
			} else if(refVector.dotProduct(velocity)/velocity.length()<-0.999999) {
				q.w = 0;
				q.x = 1;
				q.y = 0;
				q.z = 0;
			} else {
				Ogre::Vector3 cross_value = refVector.crossProduct(velocity);
				q.w = sqrt((refVector.length()*refVector.length()) + (velocity.length()*velocity.length())) + refVector.dotProduct(velocity);
				q.x = cross_value.x;
				q.y = cross_value.y;
				q.z = cross_value.z;
				q.normalise();
			}
			(*m_pLaunchedRocket).get()->setOrientation(q.w, q.x, q.y, q.z);
		}
	}
	
	//TODO end
}

void World::registerRocket(Rocket* rocket)
{
	shared_ptr<Object> r;
	r.reset(rocket);
	m_pWorld.push_back(r);
	m_pLaunchedRocket = m_pWorld.end();
	m_pLaunchedRocket--;
}
void World::launchRocket()
{
	(*m_pLaunchedRocket).get()->getRigidBody()->setLinearVelocity(btVector3(0,0,50));
	isRocketLaunched = true;	
}

void World::reloadRocket()
{
	Rocket* rocket = new Rocket("RocketNode", 11, 60, 11, 10, btVector3(0,150,0), btQuaternion((double)sqrt(2.0), 0, 0, (double)sqrt(2.0)));
	registerRocket(rocket);
}
void World::destroyRocket()
{
	//Pseudocode section!
	//globalKaboom!(); -> to be implemented soon!
	//
	//Pseudocode section ends.
	//	if(isCollidedWithTarget?) {
	//		gameClear();
	//	}
	//	else {
	//		trialFailed();
	//	}
	//the following code is failed case : generate new rocket, and try again!
	isRocketLaunched = false;
	m_pWorld.erase(m_pLaunchedRocket);
	reloadRocket();
	//ICGAppFrame::getInstance()->deleteFromDynamicsWorld(m_pLaunchedRocket.get()->getRigidBody());
	//m_pLaunchedRocket.reset();
}

bool World::isRocketFired()
{
	return isRocketLaunched;
}

Ogre::Vector3 World::getRocketPosition()
{
	if(isRocketLaunched)
	{
		return ((*m_pLaunchedRocket).get())->getPosition();
	}
	return Ogre::Vector3(0,0,0);
}

Ogre::Quaternion World::getRocketOrientation()
{
	if(isRocketLaunched)
	{
		return ((*m_pLaunchedRocket).get())->getOrientation();
	}
	return Ogre::Quaternion(0, 0, 0, 1);
}