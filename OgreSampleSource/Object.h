#include "DrawableObject.h"
#include "SimulationObject.h"

class Object : public DrawableObject, public SimulationObject
{
protected:

public:
	virtual void draw();
	virtual void stepSimulation();
};