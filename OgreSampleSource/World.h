#ifndef __WORLD__
#define __WORLD__
#include "Object.h"
#include "rocket.h"

#include <list>
#include <memory>

using std::list;
using std::shared_ptr;
class World
{
private:
	static World* instance;
	World();
	list <shared_ptr<Object> > m_pWorld;
	shared_ptr<Rocket> m_pLaunchedRocket;
public:
	static World* getInstance();
	void addObject(Object* object);
	void stepSimulation();
	void launchRocket(Rocket* rocket);
	void destroyRocket();
};
#endif