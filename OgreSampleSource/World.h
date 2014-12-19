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
	list<shared_ptr<Object>>::iterator m_pLaunchedRocket;
	bool isRocketLaunched;
public:
	static World* getInstance();
	void addObject(Object* object);
	void stepSimulation();
	void registerRocket(Rocket* rocket);
	void launchRocket();
	void destroyRocket();
};
#endif