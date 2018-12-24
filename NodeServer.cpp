#include "NodeServer.h"
#include "ServerImp.h"

using namespace std;

NodeServer g_app;

void NodeServer::initialize()
{

	string sServerObj = ServerConfig::servantName; 
	addServant<ServerImp>(sServerObj);
}

void NodeServer::destroyApp()
{

}


int main(int argc, char* argv[])
{
	ServerConfig::Application  = "tars";
    ServerConfig::ServerName   = "tarsnode";
    //ServerConfig::Node         = "127.0.0.1:19386";
    ServerConfig::OpenCoroutine = true;

    ServerConfig::adapterName = "tars.tarsnode.NodeAdapter";
    ServerConfig::servantName = "tars.tarsnode.NodeObj";
    ServerConfig::adapterIp = "127.0.0.1";
    ServerConfig::adapterPort = 19386;

    g_app.main(argc,argv);
    
    g_app.waitForShutdown();

    return -1; 
}
