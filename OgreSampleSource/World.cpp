#include "World.h"
#include <memory>
#include <typeinfo>
using std::shared_ptr;

FILE *simulLog = fopen("output.log","wt");


World::World()
	: m_pWorld(list<shared_ptr<Object> >())
	, isRocketLaunched(false)
	, isContactedWithPlanet(false)
	, isContactedWithTarget(false)
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
	m_pLaunchedRocket->get()->stepSimulation();
	for(auto it = m_pWorld.begin(); it != m_pWorld.end(); it++)
	{		
		if(!(*it)) {
			m_pWorld.erase(it);
			continue;
		}
		//if(it==m_pLaunchedRocket) continue;
		//ICGAppFrame::getInstance()->collisionCheck(m_pLaunchedRocket->get()->getRigidBody(), it->get()->getRigidBody());
	}
	ICGAppFrame::getInstance()->collisionCheck();
	//TODO : Collision detection for m_pLaunchedRocket and m_pWorld[i].
	if(isRocketLaunched) {
		if(isContactedWithTarget || isContactedWithPlanet)
		{
			destroyRocket();
			return;
		}
		Ogre::Vector3 l = (*m_pLaunchedRocket).get()->getPosition();
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
				if(dist.length()<=1000.0f) {
					double r = dist.length();
					dist = dist / dist.length();
					btVector3 forceApplied = (1000000.0/(r*r)) * btVector3(dist.x, dist.y, dist.z);
					netForce += forceApplied;
				}
			}
		}
		btVector3 v = ((Rocket*)((*m_pLaunchedRocket).get()))->getLinearVelocity();
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
	(*m_pLaunchedRocket).get()->getRigidBody()->activate();
	(*m_pLaunchedRocket).get()->getRigidBody()->setLinearVelocity(btVector3(0,0,50));
	isRocketLaunched = true;	
}

void World::reloadRocket()
{
	Rocket* rocket = new Rocket("RocketNode", 100, 100, 100, 10, btVector3(0,0,0), btQuaternion((double)sqrt(2.0), 0, 0, (double)sqrt(2.0)));
	registerRocket(rocket);
}
void World::destroyRocket()
{
	Ogre::Vector3 p = m_pLaunchedRocket->get()->getPosition();
	fprintf(simulLog, "Rocket destroyed : (%lf, %lf, %lf)\n", p.x, p.y, p.z);
	//Kaboom()
	if(isContactedWithTarget)
	{
		isContactedWithTarget = false;
		isContactedWithPlanet = false;
		//gameClear()
	}
	else if(isContactedWithPlanet)
	{
		isContactedWithPlanet = false;
		isRocketLaunched = false;
		Sleep(500);
		ICGAppFrame::getInstance()->initCamera();
		m_pWorld.erase(m_pLaunchedRocket);
		reloadRocket();
	}
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

void World::contactedWithPlanet()
{
	isContactedWithPlanet = true;
}

void World::contactedWithTarget()
{
	isContactedWithTarget = true;
}