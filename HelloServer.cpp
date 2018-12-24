#include "HelloServer.h"
#include "HelloImp.h"

using namespace std;

HelloServer g_app;

void HelloServer::initialize()
{
	//string servantName = "TestApp.HelloServer.HelloObj";

	addServant<HelloImp>(ServerConfig::servantName);	

}


void HelloServer::destroyApp()
{

}


int main(int argc, char* argv[])
{
	ServerConfig::Application  = "TestApp";
    ServerConfig::ServerName   = "HelloServer";
    ServerConfig::Node         = "127.0.0.1:19386";
    ServerConfig::OpenCoroutine = true;

	ServerConfig::adapterName = "TestApp.HelloServer.HelloObjAdapter";
	ServerConfig::servantName = "TestApp.HelloServer.HelloObj";
	ServerConfig::adapterIp = "127.0.0.1";
	ServerConfig::adapterPort = 9877;

	g_app.main(argc,argv);
	
	g_app.waitForShutdown();

	return -1;
}
