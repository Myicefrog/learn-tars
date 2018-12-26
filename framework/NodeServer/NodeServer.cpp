#include "NodeServer.h"
#include "ServerImp.h"
#include "NodeImp.h"

using namespace std;

NodeServer g_app;

void NodeServer::initialize()
{

	string sServerObj = ServerConfig::servantName; 
	addServant<ServerImp>(sServerObj);

	string sServerObj1 = ServerConfig::servantName1;
	addServant<NodeImp>(sServerObj1);
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

    ServerConfig::adapterName = "tars.tarsnode.ServerAdapter";
    ServerConfig::servantName = "tars.tarsnode.ServerObj";
    ServerConfig::adapterIp = "127.0.0.1";
    ServerConfig::adapterPort = 19386;

    ServerConfig::adapterName1 = "tars.tarsnode.NodeAdapter";
    ServerConfig::servantName1 = "tars.tarsnode.NodeObj";
    ServerConfig::adapterIp1 = "127.0.0.1";
    ServerConfig::adapterPort1 = 19385;

    g_app.main(argc,argv);
    
    g_app.waitForShutdown();

    return -1; 
}
