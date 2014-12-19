#ifndef __WORLD__
#define __WORLD__
#include "Object.h"
#include <vector>
#include <memory>
#include "rocket.h"

using std::vector;
using std::shared_ptr;
class World
{
private:
	static World* instance;
	World();
	vector <Object*> m_pWorld;
	shared_ptr<Rocket> m_pLaunchedRocket;
public:
	static World* getInstance();
	void addObject(Object* object);
	void stepSimulation();
	void launchRocket(Rocket* rocket);
	void destroyRocket();
};
#endif