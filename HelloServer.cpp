#include "HelloServer.h"
#include "HelloImp.h"

using namespace std;

HelloServer g_app;

void HelloServer::initialize()
{
	string servantName = "TestApp.HelloServer.HelloObj";

	addServant<HelloImp>(servantName);	

}


void HelloServer::destroyApp()
{

}


int main(int argc, char* argv[])
{
	g_app.main(argc,argv);
	
	g_app.waitForShutdown();

	return -1;
}
