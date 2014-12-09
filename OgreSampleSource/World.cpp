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
	for(auto it = m_pWorld.begin(); it != m_pWorld.end(); it++)
	{
		(*it)->stepSimulation();
	}
}
