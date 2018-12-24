#ifndef __SERVER_IMP_H_
#define __SERVER_IMP_H_
#include "NodeF.h"
#include <unistd.h>
#include "ServantProxy.h"
#include "Servant.h"
#include <memory>


using namespace tars;
using namespace std;

class ServerImp : public ServerF
{
public:
    /**
     * 构造函数
     */
    ServerImp()
    {
    }


    /**
     * 析构函数
     */
    virtual ~ServerImp()
    {
    }

    /**
     * 初始化
     */
    virtual void initialize()
    {
    };

    /**
     * 退出
     */
    virtual void destroy()
    {
    };

    /**
     * 上报心跳
     */
    virtual int keepAlive( const std::string & sReq,vector<char> &buffer ) ;
    
private:
};

typedef shared_ptr<ServerImp> ServerImpPtr;

#endif
