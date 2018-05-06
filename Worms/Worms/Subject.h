#pragma once
#include "Observer.h"
#include <vector>

/*
Clase sujeto. Posee una lista de observadores registrados, a los cuales notifica cada vez
que se realiza un cambio en el sujeto. Un ejemplo de sujeto sería la clase Scenario, la cual
notifica a los observadores cada vez que un Worm se movio.

Contiene metodos para registrar y desregistrar observadores, ademas de la funcion update, la cual informa
a todos los observadores registrados.
*/

struct lastAction {
	unsigned int id;
	unsigned int origin;
};

class Subject
{
public:
	Subject();
	virtual ~Subject();
	virtual void registerObserver(Observer* observer);
	virtual void unregisterObserver(Observers index);
	lastAction getLastAction();

protected:
	std::vector<Observer*> observerList;
	virtual void notify(void* tool);
	lastAction lastAction;
};

