#ifndef __NODE_SERVER_H_
#define __NODE_SERVER_H_

#include <iostream>
#include "Application.h"

using namespace tars;

class NodeServer : public Application
{
public:

    virtual ~NodeServer(){}; 

    virtual void initialize();

    virtual void destroyApp();

};

extern NodeServer g_app;

#endif
