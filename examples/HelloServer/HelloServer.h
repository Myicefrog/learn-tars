#ifndef _HelloServer_H_
#define _HelloServer_H_

#include <iostream>
#include "Application.h"

using namespace tars;

class HelloServer : public Application
{
public:
	
	virtual ~HelloServer() {};

	virtual void initialize();

	virtual void destroyApp();
};

extern HelloServer g_app;

#endif
