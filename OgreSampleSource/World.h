#ifndef __WORLD__
#define __WORLD__
#include "Object.h"
#include <vector>
using std::vector;
class World
{
private:
	static World* instance;
	World();
	vector <Object*> m_pWorld;
public:
	static World* getInstance();
	void addObject(Object* object);
	void stepSimulation();
};
#endif