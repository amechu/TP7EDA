#pragma once
#include "Observer.h"
#include "AllegroTools.h"
#include "Scenario.h"

class Drawer : public Observer
{
public:
	Drawer();
	~Drawer();
	void update(void* subject, void* drawingTool);
	void update(void* subject); //&0 ver que hacer con esto si sacarle el virtual pure a observer
};

