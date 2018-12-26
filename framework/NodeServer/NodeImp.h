#ifndef __NODE_IMP_H_
#define __NODE_IMP_H_

#include "Node.h"

using namespace tars;
using namespace std;

class NodeImp : public Node
{
public:

	NodeImp()
	{

	}

	~NodeImp()
    {
    }

	virtual void initialize();

    virtual void destroy()
    {
    }

    virtual int startServer( const std::string & sReq,vector<char> &buffer ) ;
	
};

typedef shared_ptr<NodeImp> NodeImpPtr;

#endif
