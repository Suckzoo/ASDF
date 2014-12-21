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
	, isExploding(false)
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
		Ogre::Real d = refVector.dotProduct(velocity)/velocity.length();
		if(velocity.length()>0.0001) {
			velocity /= velocity.length();
			if(d>0.999999) {
				q.w = 1;
				q.x = 0;
				q.y = 0;
				q.z = 0;
			} else if(d<-0.999999) {
				q.w = 0;
				q.x = 1;
				q.y = 0;
				q.z = 0;
			} else {
				Ogre::Real s = sqrt( (1+d)*2 );
				Ogre::Real invs = 1 / s;

				Ogre::Vector3 c = refVector.crossProduct(velocity);
				q.x = c.x * invs;
				q.y = c.y * invs;
				q.z = c.z * invs;
				q.w = s * 0.5f;
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
void World::launchRocket(btVector3 direction)
{
	(*m_pLaunchedRocket).get()->getRigidBody()->activate();
	(*m_pLaunchedRocket).get()->getRigidBody()->setLinearVelocity(50*direction);
	isRocketLaunched = true;	
}

void World::reloadRocket()
{
	Rocket* rocket = new Rocket("RocketNode", 11, 60, 11, 10, btVector3(0,0,0), btQuaternion((double)sqrt(2.0), 0, 0, (double)sqrt(2.0)));
	registerRocket(rocket);
}
void World::destroyRocket()
{
	Ogre::Vector3 p = m_pLaunchedRocket->get()->getPosition();
	fprintf(simulLog, "Rocket destroyed : (%lf, %lf, %lf)\n", p.x, p.y, p.z);
	//Kaboom()
	if(!isExploding){
		((Rocket*)((*m_pLaunchedRocket).get()))->deleteTailEffect();
		Ogre::SceneManager* sceneMgr = ICGAppFrame::getInstance()->getSceneMgr();
		Ogre::ParticleSystem* particleSystem1 = sceneMgr->createParticleSystem("ExplosionEffect1", "Examples/ModifiedFountain");
		particleSystem1->fastForward(10.0);
		(*m_pLaunchedRocket).get()->getSceneNode()->attachObject(particleSystem1);
		Ogre::ParticleSystem* particleSystem2 = sceneMgr->createParticleSystem("ExplosionEffect2", "Examples/ModifiedSmoke");
		particleSystem2->fastForward(10.0);
		(*m_pLaunchedRocket).get()->getSceneNode()->attachObject(particleSystem2);
		isExploding = true;
		explosion_begin = explosion_end = clock();
		return;
	}
	else if(explosion_end - explosion_begin < 5000){
		explosion_end = clock();
		return;
	}
	else{
		isExploding = false;
		ICGAppFrame::getInstance()->getSceneMgr()->destroyParticleSystem("ExplosionEffect1");
		ICGAppFrame::getInstance()->getSceneMgr()->destroyParticleSystem("ExplosionEffect2");
	}

	if(isContactedWithTarget)
	{
		isContactedWithTarget = false;
		isContactedWithPlanet = false;
		ICGAppFrame::getInstance()->Shutdown();
		//gameClear()
	}
	else if(isContactedWithPlanet)
	{
		isContactedWithPlanet = false;
		isRocketLaunched = false;
		ICGAppFrame::getInstance()->setPhase(1);
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

void World::setRocketPosition(Ogre::Real x, Ogre::Real y, Ogre::Real z)
{
	m_pLaunchedRocket->get()->setPosition(x, y, z);
}

void World::setRocketOrientation(Ogre::Real w, Ogre::Real x, Ogre::Real y, Ogre::Real z)
{
	m_pLaunchedRocket->get()->setOrientation(w, x, y, z);
}

Rocket* World::getRocket()
{
	return (Rocket*)(*m_pLaunchedRocket).get();
}

void World::contactedWithPlanet()
{
	isContactedWithPlanet = true;
}

void World::contactedWithTarget()
{
	isContactedWithTarget = true;
}