#include "AdminServant.h"
#include "Application.h"


namespace tars
{

AdminServant::AdminServant()
{
}

AdminServant::~AdminServant()
{
}

void AdminServant::initialize()
{
	cout<<"AdminServant::initialize()"<<endl;
}

void AdminServant::destroy()
{
}

void AdminServant::shutdown()
{
    Application::terminate();
}

string AdminServant::notify(const string &command)
{
	cout<<"dminServant::notify"<<endl;
	return "notify";
}

}
