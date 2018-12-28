#include "Application.h"

using namespace tars;
using namespace std;

class AdminRegistryServer : public Application
{
protected:

	virtual void initialize();

	virtual void destroyApp();

};

extern AdminRegistryServer g_app;

