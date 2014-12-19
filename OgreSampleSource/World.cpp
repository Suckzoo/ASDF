#include "World.h"
#include <memory>
#include <typeinfo>
using std::shared_ptr;

FILE *simulLog = fopen("output.log","wt");

World::World()
	: m_pWorld(list<shared_ptr<Object> >()), isRocketLaunched(false)
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
		fprintf(simulLog, "Rocket launched.\n");
		Ogre::Vector3 l = (*m_pLaunchedRocket).get()->getPosition();
		fprintf(simulLog, "Current rocket position : (%lf, %lf, %lf)\n", l.x, l.y, l.z);
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
						fprintf(simulLog, "Force applied by planet : (%lf, %lf, %lf)\n", forceApplied.x(), forceApplied.x(), forceApplied.x());
					//}
				}
			}
		}
		fprintf(simulLog, "Total force applied is : (%lf, %lf, %lf)\n", netForce.x(), netForce.y(), netForce.z());
		btVector3 v = ((Rocket*)((*m_pLaunchedRocket).get()))->getLinearVelocity();
		fprintf(simulLog, "Current velocity of rocket is : (%lf, %lf, %lf)\n", v.x(), v.y(), v.z());
		((Rocket*)(*m_pLaunchedRocket).get())
			->applyCentralForce(netForce.x(), netForce.y(), netForce.z());
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
	isRocketLaunched = true;	
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
	registerRocket(new Rocket("RocketNode", 500, 500, 500, 10, btVector3(-100, 0, 0)));
	//ICGAppFrame::getInstance()->deleteFromDynamicsWorld(m_pLaunchedRocket.get()->getRigidBody());
	//m_pLaunchedRocket.reset();
}
