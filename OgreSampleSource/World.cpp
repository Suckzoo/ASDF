#include "World.h"
#include <memory>
using std::shared_ptr;
World::World()
	: m_pWorld(list<shared_ptr<Object> >())
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
		
		/*if() {
			m_pWorld.erase(it);
			continue;
		}*/
		(*it)->stepSimulation();
	}
	//TODO : Collision detection for m_pLaunchedRocket and m_pWorld[i].

	//TODO end
}

void World::launchRocket(Rocket* rocket)
{
	m_pLaunchedRocket.reset(rocket);
}

void World::destroyRocket()
{
	ICGAppFrame::getInstance()->deleteFromDynamicsWorld(m_pLaunchedRocket.get()->getRigidBody());
	m_pLaunchedRocket.reset();
}
