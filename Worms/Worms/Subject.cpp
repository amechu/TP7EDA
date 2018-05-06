#include "Subject.h"


Subject::Subject()
{
}


Subject::~Subject()
{
}

void Subject::notify(void* tool)
{
	for (Observer* observer : this->observerList) {
		observer->update(this, tool);
	}
}

void Subject::registerObserver(Observer * observer)
{
	this->observerList.push_back(observer);
}

void Subject::unregisterObserver(Observers index)
{
	this->observerList.erase(observerList.begin() + (int) index);
}

lastAction Subject::getLastAction()
{
	return this->lastAction;
}
