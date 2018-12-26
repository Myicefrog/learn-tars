#ifndef __TARS_SERVANT_HELPER_H
#define __TARS_SERVANT_HELPER_H

#include <iostream>
#include <map>
#include <vector>
#include "tc_singleton.h"
#include "Servant.h"

namespace tars
{

class ServantHelperCreation
{
public:
    virtual ServantPtr create(const string &s) = 0;
};
typedef shared_ptr<ServantHelperCreation> ServantHelperCreationPtr;

template<class T>
struct ServantCreation : public ServantHelperCreation
{

    ServantPtr create(const string &s) 
	{
    	cout<<"ServantPtr create "<<endl;
		shared_ptr<T> p = make_shared<T>();
		p->setName(s); 
		return p; 
	}
};

class ServantHelperManager : public TC_Singleton<ServantHelperManager>
{
public:

	ServantHelperManager(){}

    template<typename T>
    void addServant(const string &id,bool check = false)
    {
        if(check && _servant_adapter.end() == _servant_adapter.find(id))
        {
            cerr<<"[TARS]ServantHelperManager::addServant "<< id <<" not find adapter.(maybe not conf in the web)"<<endl;
            throw runtime_error("[TARS]ServantHelperManager::addServant " + id + " not find adapter.(maybe not conf in the web)");
        }
	
		shared_ptr< ServantCreation<T> > servantCreationPtr = make_shared< ServantCreation<T> >();		
        _servant_creator[id] = servantCreationPtr;
		
    }	

	ServantPtr create(const string &sAdapter);

	void setAdapterServant(const string &sAdapter, const string &sServant);

	string getAdapterServant(const string &sAdapter)
    {
        map<string, string>::iterator it = _adapter_servant.find(sAdapter);
        if(it != _adapter_servant.end())
        {
            return it->second;
        }
        return "(NO TARS PROTOCOL)";
    }
	
	string getServantAdapter(const string& sServant)
    {
        map<string, string>::iterator it = _servant_adapter.find(sServant);
        if(it != _servant_adapter.end())
        {
            return it->second;
        }
        return "";
    }	

	map<string, string> getAdapterServant() {return _adapter_servant;}

protected:

	map<string, ServantHelperCreationPtr>   _servant_creator;

	map<string, string>                     _adapter_servant;

	map<string, string>                     _servant_adapter;	

	TC_ThreadMutex _mutex;
};

}

#endif
