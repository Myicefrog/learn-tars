#include "AdminRegistryServer.h"
#include "AdminRegistryImp.h"

AdminRegistryServer g_app;

void AdminRegistryServer::initialize()
{
	string adminObj = ServerConfig::servantName;
	addServant<AdminRegistryImp>(adminObj);

}

void AdminRegistryServer::destroyApp()
{

}

int main(int argc, char *argv[])
{
	ServerConfig::Application  = "tars";
    ServerConfig::ServerName   = "tarsAdminRegistry";
    //ServerConfig::Node         = "127.0.0.1:19386";
    ServerConfig::OpenCoroutine = false;

    ServerConfig::adapterName = "tars.tarsAdminRegistry.AdminRegObjAdapter";
    ServerConfig::servantName = "tars.tarsAdminRegistry.AdminRegObj";
    ServerConfig::adapterIp = "127.0.0.1";
    ServerConfig::adapterPort = 12000;

	g_app.main(argc, argv);

	g_app.waitForShutdown();

	return 0;
}
