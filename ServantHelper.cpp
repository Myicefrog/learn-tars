#include "ServantHelper.h"

namespace tars
{

ServantPtr ServantHelperManager::create(const string &sAdapter)
{
    if(_adapter_servant.find(sAdapter) == _adapter_servant.end())
    {
        return NULL;
    }

    ServantPtr servant = NULL;

    //根据adapter查找servant名称
    string s = _adapter_servant[sAdapter];
    cout<<"ServantHelperManager::create servant name "<<s<<endl;

    if(_servant_creator.find(s) != _servant_creator.end())
    {
        servant = _servant_creator[s]->create(s);
    }
    return servant;
}	

void ServantHelperManager::setAdapterServant(const string &sAdapter, const string &sServant)
{
    _adapter_servant[sAdapter] = sServant;

    _servant_adapter[sServant] = sAdapter;
}

}

