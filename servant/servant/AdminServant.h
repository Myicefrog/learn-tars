#ifndef __TARS_ADMIN_SERVANT_H
#define __TARS_ADMIN_SERVANT_H

#include "AdminF.h"

namespace tars
{
class Application;

class AdminServant : public AdminF
{
public:

	AdminServant();

	virtual ~AdminServant();

	virtual void initialize();

	virtual void destroy();

	void shutdown();

	string notify(const string &command);

};

}

#endif
