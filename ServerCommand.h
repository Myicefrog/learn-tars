#ifndef __SERVER_COMMAND_H_
#define __SERVER_COMMAND_H_

#include "ServerObject.h"
#include "NodeServer.h"

class ServerCommand
{
public:

	enum ExeStatus
    {   
        EXECUTABLE,         //可以运行
        DIS_EXECUTABLE,     //不可以运行
        NO_NEED_EXECUTE     //不需要运行
    };

public:

	virtual ~ServerCommand(){}

	virtual ExeStatus canExecute(string &sResult){ return EXECUTABLE;}

	virtual int execute(string &sResult) = 0;

	virtual int doProcess()
    {
        string s ;
        return doProcess(s);
    }
	
	virtual int doProcess(string &sResult)
    {
        ExeStatus t = canExecute(sResult);
        if(t == DIS_EXECUTABLE)
        {
            return -1;
        }
        else if(t == NO_NEED_EXECUTE)
        {
            return 0;
        }

        return execute(sResult);
    }

};

#endif
