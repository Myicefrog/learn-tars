#ifndef __TARS_NODEF_H__
#define __TARS_NODEF_H__

#include "NodeF.h"
#include "Global.h"
#include "tc_singleton.h"

namespace tars
{

/**
 * 给node发送心跳
 * 调用keepAlive异步发送心跳给node
 */
class TarsNodeFHelper : public TC_Singleton<TarsNodeFHelper>,public TC_ThreadMutex 
{
public:

    /**
     * 设置node信息
     * @param comm, 通信器
     * @param obj,
     * @param app
     * @param server
     */
    void setNodeInfo(const CommunicatorPtr &comm, const string &obj, const string &app, const string &server);

    /**
     * keepAlive
     */
    void keepAlive(const string &adapter = "");

protected:
    /**
     * 通信器
     */
    CommunicatorPtr _comm;

    /**
     * Node
     */
    //ServerFPrx      _nodePrx;
	ServerFProxy*   _nodePrx;

    /**
     * 信息
     */
    ServerInfo      _si;

    set<string>     _adapterSet;

};

}

#endif
