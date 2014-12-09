#include "World.h"

World::World()
	: m_pWorld(vector<Object*>())
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
	m_pWorld.push_back(obj);
}

void World::stepSimulation()
{
	//m_pLaunchedRocket.get()->stepSimulation();
	for(auto it = m_pWorld.begin(); it != m_pWorld.end(); it++)
	{
		(*it)->stepSimulation();
	}
	//TODO : Collision detection for m_pLaunchedRocket and m_pWorld[i].
}
//
//void World::launchRocket(Rocket* rocket)
//{
//	m_pLaunchedRocket.reset(rocket);
//}
//
//void World::destroyRocket()
//{
//	ICGAppFrame::getInstance()->deleteFromDynamicsWorld(m_pLaunchedRocket.get()->getRigidBody());
//	m_pLaunchedRocket.reset();
//}